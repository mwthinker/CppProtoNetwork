#ifndef CPPPROTONETWORK_NET_CONNECTION_H
#define CPPPROTONETWORK_NET_CONNECTION_H

#include "protobufmessage.h"
#include "logger.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <functional>
#include <mutex>
#include <queue>

namespace net {
	
	using DisconnectHandler = std::function<void()>;

	class Connection {
	public:
		Connection(asio::ip::tcp::socket socket);
		~Connection();

		void send(const google::protobuf::MessageLite& message);

		asio::ip::tcp::socket socket_;

		template <typename ProtocolMessage>
		void setReceiveHandler(std::function<void(const ProtocolMessage&)> messageHandler) {
			static_assert(std::is_base_of<google::protobuf::MessageLite, ProtocolMessage>::value,
				"template type must have google::protobuf::MessageLite as base class");
			
			ProtocolMessage protocolMessage;
			receiveHandler_ = [protocolMessage, messageHandler]
			(const net::ProtobufMessage& message) mutable {
				protocolMessage.Clear();
				bool valid = protocolMessage.ParseFromArray(message.getBodyData(), message.getBodySize());
				if (valid) {
					messageHandler(protocolMessage);
				} else {
					logger()->error("[Connection] Protocol error");
				}
				
			};
		}

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler) {
			disconnectHandler_ = disconnectHandler;
		}

		void readHeader();

		void disconnect();

	protected:
		void sendHeader(const google::protobuf::MessageLite& message);
		void sendBody(net::ProtobufMessage&& protobufMessage);

		using ReceiveHandler = std::function<void(const net::ProtobufMessage& message)>;

		void readBody();

		ReceiveHandler receiveHandler_;
		DisconnectHandler disconnectHandler_;
		std::queue<net::ProtobufMessage> sendBuffer_;

		

		//std::deque<net::ProtobufMessage> sendBuffer_;
		net::ProtobufMessage receiveBuffer_;
		mutable std::mutex mutex_;
		uint16_t receivedSize_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CONNECTION_H
