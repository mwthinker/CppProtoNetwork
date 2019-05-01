#include "server.h"

using namespace asio;
using asio::ip::tcp;

namespace net {

	std::shared_ptr<Server> Server::create() {
		return std::shared_ptr<Server>(new Server);
	}

	Server::Server() : socket_(ioService_),
		closeConnection_(false), allowConnections_(false),
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
				allowConnections_ = true;
			} catch (asio::system_error e) {
				throw;
			}
			thread_ = std::thread([keapAlive = shared_from_this()]() {
				keapAlive->doAccept();
				keapAlive->ioService_.run();
			});
		}
	}

	void Server::disconnect() {
		allowConnections_ = false;
		for (auto& client : clients_) {
			client->disconnect();
		}
		clients_.clear();
		
		ioService_.stop();
		thread_.join();
	}

	void Server::setAllowingNewConnections(bool allow) {
		if (allow != allowConnections_) {
			allowConnections_ = allow;
		}
	}

	bool Server::isAllowingNewConnections() const {
		return allowConnections_;
	}

	void Server::sendToAll(const google::protobuf::MessageLite& message) {
		for (auto& client : clients_) {
			client->send(message);
		}
	}

	void Server::doAccept() {
		acceptor_.async_accept(socket_, [keapAlive = shared_from_this()](std::error_code ec) {
			if (!ec && keapAlive->allowConnections_) {
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
