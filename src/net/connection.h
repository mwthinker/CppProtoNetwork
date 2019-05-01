#ifndef CPPPROTONETWORK_NET_CONNECTION_H
#define CPPPROTONETWORK_NET_CONNECTION_H

#include "protobufmessage.h"
#include "bufferqueue.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <functional>
#include <mutex>
#include <queue>

namespace net {

	enum Error {
		NO,
		MESSAGE_MAX_SIZE,
		PROTOBUF_PROTOCOL_ERROR
	};

	struct ConnectionErrorCategory : std::error_category
	{
		const char* name() const noexcept override;
		std::string message(int ev) const override;
	};

	std::error_code make_error_code(Error e);
	
	using DisconnectHandler = std::function<void(std::error_code ec)>;
	using ReceiveHandler = std::function<bool(const net::ProtobufMessage& message, std::error_code ec)>;

	class Connection {
	public:
		Connection(asio::ip::tcp::socket socket);
		~Connection();

		void send(const google::protobuf::MessageLite& message);

		asio::ip::tcp::socket socket_;

		template <typename ProtocolMessage>
		void setReceiveHandler(std::function<bool(const ProtocolMessage&, std::error_code)> messageHandler) {
			static_assert(std::is_base_of<google::protobuf::MessageLite, ProtocolMessage>::value,
				"template type must have google::protobuf::MessageLite as base class");
			
			ProtocolMessage protocolMessage;
			receiveHandler_ = [protocolMessage, messageHandler]
			(const net::ProtobufMessage& message, std::error_code ec) mutable -> bool {
				protocolMessage.Clear();
				bool valid = protocolMessage.ParseFromArray(message.getBodyData(), message.getBodySize());
				if (valid) {
					return messageHandler(protocolMessage, make_error_code(Error::NO));
				} else {
					return messageHandler(protocolMessage, make_error_code(Error::PROTOBUF_PROTOCOL_ERROR));
				}
			};
		}

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler) {
			disconnectHandler_ = disconnectHandler;
		}

		void readHeader();

		void disconnect(std::error_code ec);

	protected:
		void readBody();

		ReceiveHandler receiveHandler_;
		DisconnectHandler disconnectHandler_;
		BufferQueue sendBuffer_;
		net::ProtobufMessage receiveMessage_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CONNECTION_H
