#ifndef CPPPROTONETWORK_NET_SERVER_H
#define CPPPROTONETWORK_NET_SERVER_H

#include "protobufmessage.h"
#include "remoteclient.h"
#include "remoteclient.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <atomic>
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>

namespace net {

	using RemoteClientPtr = std::shared_ptr<RemoteClient>;
	using ServerConnectHandler = std::function<void(const RemoteClientPtr& remoteClientPtr)>;

	class Server : public std::enable_shared_from_this<Server> {
	public:
		friend class RemoteClient;

		~Server();

		static std::shared_ptr<Server> create();

		void connect(int port);

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
		Server();

		void removeClient(const RemoteClientPtr& client);

		void doAccept();

		ServerConnectHandler connectHandler_;

		bool closeConnection_;
		int port_;
		std::atomic<bool> active_;
		std::atomic<bool> allowConnections_;
		asio::io_service ioService_;
		asio::ip::tcp::socket socket_;
		asio::ip::tcp::acceptor acceptor_;
		std::vector<RemoteClientPtr> clients_;
		std::mutex mutex_;
		std::thread thread_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_SERVER_H
