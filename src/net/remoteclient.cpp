#include "remoteclient.h"

#include <asio.hpp>

namespace net {

	std::shared_ptr<RemoteClient> RemoteClient::create(asio::ip::tcp::socket socket) {
		return std::shared_ptr<RemoteClient>(new RemoteClient(std::move(socket)));
	}

	RemoteClient::RemoteClient(asio::ip::tcp::socket socket) : connection_(std::move(socket)) {
		connection_.readHeader();
	}

	void RemoteClient::send(const google::protobuf::MessageLite& message) {
		connection_.send(message);
	}

	void RemoteClient::disconnect() {
		connection_.disconnect(make_error_code(Error::NONE));
	}

} // Namespace net.
