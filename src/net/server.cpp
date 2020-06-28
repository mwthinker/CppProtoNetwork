#include "server.h"

using namespace asio;
using asio::ip::tcp;

namespace net {

	namespace {

		template<class T>
		void removeFirstMatch(std::vector<T>& v, const T& value) {
			auto it = std::find(v.begin(), v.end(), value);
			if (it != v.end()) {
				std::swap(*it, v.back());
				v.pop_back();
			}
		}

	}

	std::shared_ptr<Server> Server::create(asio::io_context& ioContext) {
		return std::shared_ptr<Server>(new Server{ioContext});
	}

	Server::Server(asio::io_context& ioContext)
		: ioContext_{ioContext}
		, socket_{ioContext}
		, acceptor_{ioContext} {
		
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Server::~Server() {
		disconnect();
	}

	void Server::connect(unsigned short port) {
		asio::post(ioContext_, [this, port]() {
			if (!active_ && port_ == 0 && !acceptor_.is_open()) {
				active_ = true;
				port_ = port;
				try {
					acceptor_ = asio::ip::tcp::acceptor{ioContext_, tcp::endpoint{tcp::v4(), port}, false};
					allowConnections_ = true;
				} catch (asio::system_error e) {
					active_ = false;
					if (disconnectHandler_) {
						disconnectHandler_(e);
					}
					return;
				}

				doAccept();
			}
		});
	}

	void Server::disconnect() {
		asio::post(ioContext_, [this]() {
			if (active_) {
				allowConnections_ = false;
				for (auto& client : clients_) {
					client->disconnect();
				}
				clients_.clear();
				active_ = false;
			}
		});
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
			for (auto& client : clients_) {
				client->send(message);
			}
		}
	}

	void Server::doAccept() {
		acceptor_.async_accept(socket_, [keapAlive = shared_from_this()](std::error_code ec) {
			if (!ec && keapAlive->allowConnections_) {
				auto remoteClient = RemoteClient::create(std::move(keapAlive->socket_), keapAlive);
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
		removeFirstMatch<RemoteClientPtr>(clients_, client);
	}

}
