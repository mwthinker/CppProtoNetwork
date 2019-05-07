#include "client.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <iostream>

using namespace asio;
using asio::ip::tcp;

namespace net {

	Client::Client() : connection_(asio::ip::tcp::socket(ioService_)), active_(false) {
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	void Client::send(const google::protobuf::MessageLite& message) {
		if (active_) {
			connection_.send(message);
		}
	}

	void Client::disconnect() {
		connection_.disconnect(make_error_code(Error::NONE));
		active_ = false;
	}

	void Client::connect(const std::string& ip, int port) {
		if (!active_) {
			active_ = true;
			asio::ip::tcp::endpoint endpoint(ip::address::from_string(ip), port);
			tcp::resolver resolver(ioService_);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(endpoint);

			asio::async_connect(connection_.getSocket(), endpoint_iterator,
				[keapAlive = shared_from_this()](std::error_code ec, tcp::resolver::iterator) {

				if (keapAlive->connectHandler_) {
					keapAlive->connectHandler_(ec);
				}
				if (!ec) {
					keapAlive->connection_.readHeader();
				}
			});
			thread_ = std::thread([keapAlive = shared_from_this()]() {
				keapAlive->ioService_.run();
			});
		}
	}

	Client::~Client() {
		if (active_) {
			ioService_.stop();
			thread_.join();
		}
	}

	std::shared_ptr<Client> Client::create() {
		return std::shared_ptr<Client>(new Client);
	}

} // Namespace net.
