#include "server.h"
#include "logger.h"

using namespace asio;
using asio::ip::tcp;

namespace net {

	std::shared_ptr<Server> Server::create() {
		return std::shared_ptr<Server>(new Server);
	}

	Server::Server() : socket_(ioService_),
		closeConnection_(false),
		acceptor_(ioService_), port_(0) {

		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Server::~Server() {
		disconnect();
	}

	void Server::connect(int port) {
		if (port_ == 0 && !acceptor_.is_open()) {
			port_ = port;
			try {
				acceptor_ = asio::ip::tcp::acceptor(ioService_, tcp::endpoint(tcp::v4(), port));
			} catch (asio::system_error e) {
				throw;
			}
			thread_ = std::thread([&]() {
				doAccept();
				ioService_.run();
			});
		}
	}

	void Server::disconnect() {
		for (auto& client : clients_) {
			client->disconnect();
		}
		clients_.clear();
		
		ioService_.stop();
		thread_.join();
	}

	void Server::setAllowingNewConnections(bool allow) {
		if (allow && isAllowingNewConnections()) {
			try {
				acceptor_ = asio::ip::tcp::acceptor(ioService_, tcp::endpoint(tcp::v4(), port_));
				//acceptor_.open(asio::ip::tcp::v4());
			} catch (asio::system_error e) {
				throw;
			}
			doAccept();
		} else if (!isAllowingNewConnections()) {
			std::error_code ec;
			acceptor_.close(ec);
			if (ec) {
				logger()->error("[Connection] {}", ec.message());
			}
		}
	}

	bool Server::isAllowingNewConnections() const {
		return acceptor_.is_open();
	}

	void Server::sendToAll(const google::protobuf::MessageLite& message) {
		for (auto& client : clients_) {
			client->send(message);
		}
	}

	void Server::doAccept() {
		auto keapAlive = shared_from_this();
		acceptor_.async_accept(socket_, [keapAlive](std::error_code ec) {
			if (!ec) {
				auto remoteClient = RemoteClient::create(std::move(keapAlive->socket_));
				keapAlive->clients_.push_back(remoteClient);

				if (keapAlive->connectHandler_) {
					keapAlive->connectHandler_(remoteClient);
				}
			}

			if (!keapAlive->closeConnection_) {
				keapAlive->doAccept();
			}
		});
	}

} // Namespace net.
