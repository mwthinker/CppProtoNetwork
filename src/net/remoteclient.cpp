#include "remoteclient.h"
#include "server.h"

#include <asio.hpp>

namespace net {

	RemoteClient::RemoteClient(asio::ip::tcp::socket socket, std::shared_ptr<Server> server)
		: connection_{std::move(socket)}, server_{server} {
		
		connection_.readHeader();
	}

	void RemoteClient::send(const google::protobuf::MessageLite& message) {
		connection_.send(message);
	}

	void RemoteClient::disconnect() {
		connection_.disconnect(make_error_code(Error::None));
	}

	void RemoteClient::setDisconnectHandler(DisconnectHandler&& disconnectHandler) {
		connection_.setDisconnectHandler([disconnectHandler = disconnectHandler, keapAlive = shared_from_this()](std::error_code ec) {
			keapAlive->server_->removeClient(keapAlive->shared_from_this());
			disconnectHandler(ec);
		});
	}

}
