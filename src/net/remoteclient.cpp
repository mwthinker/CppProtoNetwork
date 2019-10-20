#include "remoteclient.h"
#include "server.h"

#include <asio.hpp>

namespace net {

	std::shared_ptr<RemoteClient> RemoteClient::create(std::mutex& mutex, asio::ip::tcp::socket socket,
		const std::shared_ptr<Server>& server) {
		
		return std::shared_ptr<RemoteClient>(new RemoteClient{mutex, std::move(socket), server});
	}

	RemoteClient::RemoteClient(std::mutex& mutex, asio::ip::tcp::socket socket, const std::shared_ptr<Server>& server)
		: connection_{mutex, std::move(socket)}, server_{server} {
		
		connection_.readHeader();
	}

	void RemoteClient::send(const google::protobuf::MessageLite& message) {
		connection_.send(message);
	}

	void RemoteClient::disconnect() {
		connection_.disconnect(make_error_code(Error::NONE));
	}

	void RemoteClient::setDisconnectHandler(DisconnectHandler&& disconnectHandler) {
		std::lock_guard<std::mutex> lock{connection_.getMutex()};
		connection_.setDisconnectHandler([disconnectHandler = disconnectHandler, keapAlive = shared_from_this()](std::error_code ec) {
			keapAlive->server_->removeClient(keapAlive->shared_from_this());
			disconnectHandler(ec);
		});
	}

} // Namespace net.
