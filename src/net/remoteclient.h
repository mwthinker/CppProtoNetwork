#ifndef CPPPROTONETWORK_NET_REMOTECLIENT_H
#define CPPPROTONETWORK_NET_REMOTECLIENT_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>

namespace net {

	class RemoteClient;
	class RemoteClient : public std::enable_shared_from_this<RemoteClient> {
	public:
		static std::shared_ptr<RemoteClient> create(asio::ip::tcp::socket socket);

		void send(const google::protobuf::MessageLite& message);
		
		void setReceiveHandler(const ReceiveHandler& messageHandler) {
			connection_.setReceiveHandler(messageHandler);
		}

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler) {
			connection_.setDisconnectHandler(disconnectHandler);
		}

		void disconnect();

		void release(MessageLitePtr&& message) {
			connection_.release(std::move(message));
		}

	private:
		RemoteClient(asio::ip::tcp::socket socket);
		Connection connection_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_REMOTECLIENT_H
