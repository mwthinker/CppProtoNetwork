#include "server.h"

using tcp = asio::ip::tcp;

namespace net {

	namespace {

		template<typename T>
		void removeFirstMatch(std::vector<T>& v, const T& value) {
			auto it = std::find(v.begin(), v.end(), value);
			if (it != v.end()) {
				std::swap(*it, v.back());
				v.pop_back();
			}
		}

	}

	Server::Server(IoContext& ioContext)
		: ioContext_{ioContext.ioContext_}
		, socket_{ioContext_}
		, acceptor_{ioContext_} {
		
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	Server::~Server() {
		disconnect();
	}

	std::shared_ptr<Server> Server::create(IoContext& ioContext) {
		return std::shared_ptr<Server>(new Server{ioContext});
	}

	void Server::setDisconnectHandler(ServerDisconnectHandler&& disconnectHandler) {
		disconnectHandler_ = disconnectHandler;
	}

	void Server::connect(int port) {
		asio::post(ioContext_, [this, port]() {
			if (active_) {
				disconnectError(Error::InvalidPort);
				return;
			}

			if (!isValidPort(port)) {
				disconnectError(Error::InvalidPort);
				return;
			}

			if (acceptor_.is_open()) {
				return;
			}

			active_ = true;
			port_ = static_cast<asio::ip::port_type>(port);
			try {
				acceptor_ = tcp::acceptor{ioContext_, tcp::endpoint{tcp::v4(), port_}, false};
				allowConnections_ = true;
				doAccept();
			} catch (const asio::system_error& e) {
				active_ = false;
				disconnectError(e);
				return;
			}
		});
	}

	void Server::disconnect() {
		asio::post(ioContext_, [this]() {
			if (active_) {
				return;
			}
			
			allowConnections_ = false;
			for (auto& client : clients_) {
				client->disconnect();
			}
			clients_.clear();
			active_ = false;
		});
	}

	void Server::setConnectHandler(ServerConnectHandler&& acceptionFunction) {
		connectHandler_ = acceptionFunction;
	}

	void Server::setAllowingNewConnections(bool allow) {
		allowConnections_ = allow;
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
				auto remoteClient = std::shared_ptr<RemoteClient>(new RemoteClient(std::move(keapAlive->socket_), keapAlive));
				
				keapAlive->clients_.push_back(remoteClient);

				if (keapAlive->connectHandler_) {
					keapAlive->connectHandler_(remoteClient);
				}
			} else {
				tcp::socket socket = std::move(keapAlive->socket_); // Why??
			}

			if (!keapAlive->closeConnection_) {
				keapAlive->doAccept();
			}
		});
	}

	void Server::removeClient(const RemoteClientPtr& client) {
		removeFirstMatch<RemoteClientPtr>(clients_, client);
	}

	void Server::disconnectError(Error error) {
		if (disconnectHandler_) {
			disconnectHandler_(make_error_code(error));
		}
	}

	void Server::disconnectError(const asio::system_error& error) {
		if (disconnectHandler_) {
			disconnectHandler_(error);
		}
	}

}
