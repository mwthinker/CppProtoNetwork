#ifndef CPPPROTONETWORK_NET_REMOTECLIENT_H
#define CPPPROTONETWORK_NET_REMOTECLIENT_H

#include "protobufmessage.h"

#include "detail/connection.h"

#include <asio.hpp>

namespace net {

	class Server;

	class RemoteClient;
	class RemoteClient : public std::enable_shared_from_this<RemoteClient> {
	public:
		static std::shared_ptr<RemoteClient> create(asio::ip::tcp::socket socket, const std::shared_ptr<Server>& server);

		void send(const google::protobuf::MessageLite& message);

		void disconnect();

		void setDisconnectHandler(DisconnectHandler&& disconnectHandler);

		template <typename Message>
		void setReceiveHandler(ReceiveHandler<Message>&& receiveHandler);

	private:
		RemoteClient(asio::ip::tcp::socket socket, const std::shared_ptr<Server>& server);
		
		detail::Connection connection_;
		std::shared_ptr<Server> server_;
	};

}

template <typename Message>
void net::RemoteClient::setReceiveHandler(ReceiveHandler<Message>&& receiveHandler) {
	connection_.setReceiveHandler<Message>(std::move(receiveHandler));
}

#endif
