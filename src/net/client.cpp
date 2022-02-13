#include "client.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <iostream>

using asio::ip::tcp;

namespace net {

	Client::Client(IoContext& ioCOntext)
		: ioContext_{ioCOntext.ioContext_}
		, connection_{asio::ip::tcp::socket{ioContext_}} {
		
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Client::~Client() {
		disconnect();
	}

	void Client::connect(const std::string& ip, int port) {
		asio::post(ioContext_, [this, ip, port]() {
			if (active_) {
				connectError(net::Error::AlreadyActive);
				return;
			}

			if (!isValidPort(port)) {
				connectError(net::Error::InvalidPort);
				return;
			}
			
			active_ = true;
			std::vector<tcp::endpoint> endpointSequence{
				tcp::endpoint{asio::ip::make_address_v4(ip), static_cast<asio::ip::port_type>(port)}
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
		});
	}

	void Client::send(const google::protobuf::MessageLite& message) {
		if (active_) {
			connection_.send(message);
		}
	}

	void Client::disconnect() {
		if (active_) {
			connection_.disconnect(make_error_code(Error::None));
			disconnect();
		}
	}

	void Client::setConnectHandler(ConnectHandler&& connectHandler) {
		connectHandler_ = connectHandler;
	}

	void Client::setDisconnectHandler(DisconnectHandler&& disconnectHandler) {
		connection_.setDisconnectHandler([disconnectHandler = disconnectHandler, keapAlive = shared_from_this()](std::error_code ec) {
			keapAlive->close();
			disconnectHandler(ec);
		});
	}

	void Client::close() {
		active_ = false;
	}

	std::shared_ptr<Client> Client::create(IoContext& ioContext) {
		return std::shared_ptr<Client>(new Client{ioContext});
	}

	void Client::connectError(Error error) {
		if (connectHandler_) {
			connectHandler_(make_error_code(net::Error::InvalidPort));
		}
	}

}
