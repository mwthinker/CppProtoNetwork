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

namespace net {

	using RemoteClientPtr = std::shared_ptr<RemoteClient>;
	using ServerConnectHandler = std::function<void(const RemoteClientPtr& remoteClientPtr)>;

	class Server : public std::enable_shared_from_this<Server> {
	public:
		~Server();

		static std::shared_ptr<Server> create();

		void connect(int port);

		void disconnect();

		void setConnectHandler(const ServerConnectHandler& acceptionFunction) {
			connectHandler_ = acceptionFunction;
		}

		void sendToAll(const google::protobuf::MessageLite& message);

		const std::vector<RemoteClientPtr>& getClients() const {
			return clients_;
		}

		void setAllowingNewConnections(bool allow);

		bool isAllowingNewConnections() const;
		
	private:
		Server();

		void doAccept();

		ServerConnectHandler connectHandler_;

		asio::io_service ioService_;
		asio::ip::tcp::socket socket_;
		asio::ip::tcp::acceptor acceptor_;
		std::vector<RemoteClientPtr> clients_;
		std::thread thread_;
		bool closeConnection_;
		std::mutex mutex_;
		int port_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_SERVER_H
