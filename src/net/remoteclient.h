#ifndef CPPPROTONETWORK_NET_REMOTECLIENT_H
#define CPPPROTONETWORK_NET_REMOTECLIENT_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>

namespace net {

	class Server;

	class RemoteClient;
	class RemoteClient : public std::enable_shared_from_this<RemoteClient> {
	public:
		static std::shared_ptr<RemoteClient> create(asio::ip::tcp::socket socket, const std::shared_ptr<Server>& server);

		void send(const google::protobuf::MessageLite& message);

		void disconnect();

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler);

		template <class Message>
		void setReceiveHandler(ReceiveHandler<Message>&& receiveHandler) {
			connection_.setReceiveHandler<Message>(std::forward<ReceiveHandler<Message>>(receiveHandler));
		}

	private:
		RemoteClient(asio::ip::tcp::socket socket, const std::shared_ptr<Server>& server);
		Connection connection_;
		std::shared_ptr<Server> server_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_REMOTECLIENT_H
