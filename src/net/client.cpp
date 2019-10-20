#include "client.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <iostream>

using namespace asio;
using asio::ip::tcp;

namespace net {

	Client::Client() : connection_{mutex_, asio::ip::tcp::socket{ioContext_}} {
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Client::~Client() {
		disconnect();
		thread_.join();
	}

	void Client::connect(const std::string& ip, int port) {
		if (!active_) {
			active_ = true;
			std::vector<tcp::endpoint> endpointSequence = {
				{ ip::make_address_v4("146.20.110.250"), 80 }
			};

			tcp::resolver resolver{ioContext_};

			asio::async_connect(connection_.getSocket(), endpointSequence,
				[keapAlive = shared_from_this()](const std::error_code& ec, const tcp::endpoint& ep) {

				if (keapAlive->connectHandler_) {
					keapAlive->connectHandler_(ec);
				}
				if (ec) {
					keapAlive->connection_.disconnect(ec);
				} else {
					keapAlive->connection_.readHeader();
				}
			});
			thread_ = std::thread([keapAlive = shared_from_this()]() {
				keapAlive->ioContext_.run();
			});
		}
	}

	void Client::send(const google::protobuf::MessageLite& message) {
		if (active_) {
			connection_.send(message);
		}
	}

	void Client::disconnect() {
		if (active_) {
			connection_.disconnect(make_error_code(Error::NONE));
			disconnect();
		}
	}

	void Client::setConnectHandler(const ConnectHandler& connectHandler) {
		if (!active_) {
			connectHandler_ = connectHandler;
		}
	}

	void Client::setDisconnectHandler(const DisconnectHandler& disconnectHandler) {
		if (!active_) {
			connection_.setDisconnectHandler([disconnectHandler = disconnectHandler, keapAlive = shared_from_this()](std::error_code ec) {
				keapAlive->close();
				disconnectHandler(ec);
			});
		}
	}

	void Client::close() {
		if (active_) {
			ioContext_.stop();
			active_ = false;
		}
	}

	std::shared_ptr<Client> Client::create() {
		return std::shared_ptr<Client>(new Client);
	}

} // Namespace net.
