#include "server.h"

using namespace asio;
using asio::ip::tcp;

namespace net {

	std::shared_ptr<Server> Server::create() {
		return std::shared_ptr<Server>(new Server);
	}

	Server::Server() : socket_{ioContext_},	acceptor_{ioContext_} {
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Server::~Server() {
		disconnect();
	}

	void Server::connect(unsigned short port) {
		if (!active_ && port_ == 0 && !acceptor_.is_open()) {
			active_ = true;
			port_ = port;
			try {
				acceptor_ = asio::ip::tcp::acceptor{ioContext_, tcp::endpoint{tcp::v4(), port}, false};
				allowConnections_ = true;
			} catch (asio::system_error e) {
				active_ = false;
				throw;
			}
			thread_ = std::thread([keapAlive = shared_from_this()]() {
				keapAlive->doAccept();
				keapAlive->ioContext_.run();
			});
		}
	}

	void Server::disconnect() {
		if (active_) {
			allowConnections_ = false;
			for (auto& client : clients_) {
				client->disconnect();
			}
			clients_.clear();

			ioContext_.stop();
			thread_.join();
			active_ = false;
		}
	}

	void Server::setConnectHandler(const ServerConnectHandler& acceptionFunction) {
		if (!active_) {
			connectHandler_ = acceptionFunction;
		}
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
		if (active_) {
			std::lock_guard<std::mutex> lock(mutex_);
			for (auto& client : clients_) {
				client->send(message);
			}
		}
	}

	void Server::doAccept() {
		acceptor_.async_accept(socket_, [keapAlive = shared_from_this()](std::error_code ec) {
			if (!ec && keapAlive->allowConnections_) {
				auto remoteClient = RemoteClient::create(keapAlive->mutex_, std::move(keapAlive->socket_), keapAlive);
				keapAlive->clients_.push_back(remoteClient);

				if (keapAlive->connectHandler_) {
					keapAlive->connectHandler_(remoteClient);
				}
			} else {
				auto socket = std::move(keapAlive->socket_);
			}

			if (!keapAlive->closeConnection_) {
				keapAlive->doAccept();
			}
		});
	}

	void Server::removeClient(const RemoteClientPtr& client) {
		std::lock_guard<std::mutex> lock{mutex_};
		auto it = std::find(clients_.begin(), clients_.end(), client);
		if (it != clients_.end()) {
			std::swap(*it, clients_.back());
			clients_.pop_back();
		}
	}

} // Namespace net.
