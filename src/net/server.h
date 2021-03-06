#ifndef CPPPROTONETWORK_NET_SERVER_H
#define CPPPROTONETWORK_NET_SERVER_H

#include "protobufmessage.h"
#include "remoteclient.h"
#include "auxiliary.h"

#include <memory>
#include <atomic>

namespace net {

	using RemoteClientPtr = std::shared_ptr<RemoteClient>;
	using ServerConnectHandler = std::function<void(const RemoteClientPtr& remoteClientPtr)>;

	class Server : public std::enable_shared_from_this<Server> {
	public:
		friend class RemoteClient;

		~Server();

		static std::shared_ptr<Server> create(asio::io_context& ioContext);

		void connect(unsigned short port);

		void setDisconnectHandler(ServerDisconnectHandler&& disconnectHandler) {
			disconnectHandler_ = disconnectHandler;
		}

		void disconnect();

		void setConnectHandler(const ServerConnectHandler& acceptionFunction);

		void sendToAll(const google::protobuf::MessageLite& message);

		const std::vector<RemoteClientPtr>& getClients() const {
			return clients_;
		}

		void setAllowingNewConnections(bool allow);

		bool isAllowingNewConnections() const;

		bool isActive() const {
			return active_;
		}
		
	private:
		Server(asio::io_context& ioContext);

		void removeClient(const RemoteClientPtr& client);

		void doAccept();

		asio::io_context& ioContext_;
		ServerConnectHandler connectHandler_;
		ServerDisconnectHandler disconnectHandler_;
		asio::ip::tcp::socket socket_;
		asio::ip::tcp::acceptor acceptor_;
		std::vector<RemoteClientPtr> clients_;

		std::atomic<bool> active_{false};
		std::atomic<bool> allowConnections_{false};
		bool closeConnection_{false};
		int port_{};
	};

}

#endif
