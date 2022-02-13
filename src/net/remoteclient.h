#ifndef CPPPROTONETWORK_NET_REMOTECLIENT_H
#define CPPPROTONETWORK_NET_REMOTECLIENT_H

#include "protobufmessage.h"

#include "detail/connection.h"

#include <asio.hpp>

namespace net {

	class RemoteClient;
	class RemoteClient : public std::enable_shared_from_this<RemoteClient> {
	public:
		void send(const google::protobuf::MessageLite& message);

		void disconnect();

		void setDisconnectHandler(DisconnectHandler&& disconnectHandler);

		template <MessageLite Message>
		void setReceiveHandler(ReceiveHandler<Message>&& receiveHandler);

	private:
		friend class Server;

		RemoteClient(asio::ip::tcp::socket socket, std::shared_ptr<Server> server);
		
		detail::Connection connection_;
		std::shared_ptr<Server> server_;
	};

	template <MessageLite Message>
	void net::RemoteClient::setReceiveHandler(ReceiveHandler<Message>&& receiveHandler) {
		connection_.setReceiveHandler<Message>(std::move(receiveHandler));
	}

}

#endif
