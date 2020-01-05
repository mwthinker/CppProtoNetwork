#ifndef CPPPROTONETWORK_NET_CONNECTION_H
#define CPPPROTONETWORK_NET_CONNECTION_H

#include "protobufmessage.h"
#include "bufferqueue.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <functional>
#include <queue>

namespace net {

	enum class Error {
		NONE,
		MESSAGE_MAX_SIZE,
		MESSAGE_INCORRECT_SIZE,
		PROTOBUF_PROTOCOL_ERROR
	};

	struct ConnectionErrorCategory : std::error_category
	{
		const char* name() const noexcept override;
		std::string message(int ev) const override;
	};

	std::error_code make_error_code(Error e);

	class Meta {
	public:
		asio::ip::udp::endpoint endpoint_;
	};

	using DisconnectHandler = std::function<void(std::error_code ec)>;
	using ServerDisconnectHandler = std::function<void(std::system_error ec)>;
	
	template <class Message>
	using ReceiveHandler = std::function<void(const Message& message, std::error_code ec)>;

	template <class Message>
	constexpr void IS_BASE_OF_MESSAGELITE() {
		static_assert(std::is_base_of<google::protobuf::MessageLite, Message>::value,
			"template type must have google::protobuf::MessageLite as base class");
	}

	class Connection {
	public:
		Connection(asio::ip::tcp::socket socket);
		~Connection();

		void send(const google::protobuf::MessageLite& message);

		template <typename Message>
		void setReceiveHandler(ReceiveHandler<Message>&& messageHandler) {
			IS_BASE_OF_MESSAGELITE<Message>();

			Message protocolMessage;
			receiveHandler_ = [protocolMessage, messageHandler = std::forward<ReceiveHandler<Message>>(messageHandler)]
			(const net::ProtobufMessage& message, std::error_code ec) mutable {
				protocolMessage.Clear();
				if (ec) {
					messageHandler(protocolMessage, ec);
				} else {
					bool valid = protocolMessage.ParseFromArray(message.getBodyData(), message.getBodySize());
					if (valid) {
						messageHandler(protocolMessage, ec);
					} else {
						messageHandler(protocolMessage, make_error_code(Error::PROTOBUF_PROTOCOL_ERROR));
					}
				}
			};
		}

		void setDisconnectHandler(DisconnectHandler&& disconnectHandler);

		void readHeader();

		void disconnect(std::error_code ec);

		asio::ip::tcp::socket& getSocket() {
			return socket_;
		}

	private:
		using InternalReceiveHandler = std::function<void(const ProtobufMessage& message, std::error_code ec)>;

		asio::ip::tcp::socket socket_;

		void readBody();

		InternalReceiveHandler receiveHandler_;
		DisconnectHandler disconnectHandler_;
		ProtobufMessageQueue sendBuffer_;
		net::ProtobufMessage receiveMessage_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CONNECTION_H
