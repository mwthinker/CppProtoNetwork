#include "client.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <iostream>

using namespace asio;
using asio::ip::tcp;

namespace net {

	Client::Client(asio::io_context& ioContext)
		: ioContext_{ioContext}
		, connection_{asio::ip::tcp::socket{ioContext_}} {
		
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Client::~Client() {
		disconnect();
	}

	void Client::connect(const std::string& ip, unsigned short port) {
		asio::post(ioContext_, [this, ip, port]() {
			if (!active_) {
				active_ = true;
				std::vector<tcp::endpoint> endpointSequence{
					tcp::endpoint{ip::make_address_v4(ip), port}
				};

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
			}
		});
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
		active_ = false;
	}

	std::shared_ptr<Client> Client::create(asio::io_context& ioContext) {
		return std::shared_ptr<Client>(new Client{ioContext});
	}

} // Namespace net.
