#ifndef CPPPROTONETWORK_NET_DETAIL_CONNECTION_H
#define CPPPROTONETWORK_NET_DETAIL_CONNECTION_H

#include <net/auxiliary.h>
#include <net/protobufmessage.h>
#include <net/protobufmessagequeue.h>

#include <functional>

namespace net::detail {

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

#endif // CPPPROTONETWORK_NET_DETAIL_CONNECTION_H
