#ifndef CPPPROTONETWORK_NET_SERVER_H
#define CPPPROTONETWORK_NET_SERVER_H

#include "protobufmessage.h"
#include "remoteclient.h"
#include "auxiliary.h"
#include "iocontext.h"

#include <atomic>

namespace net {

	class NetFunctions {
	public:
		virtual void post() = 0;

	};

	class TcpSocket {

	};

	class TcpAcceptor {
	public:
		virtual void asyncAccept() = 0;
	};

	using RemoteClientPtr = std::shared_ptr<RemoteClient>;
	using ServerConnectHandler = std::function<void(const RemoteClientPtr& remoteClientPtr)>;

	class Server : public std::enable_shared_from_this<Server> {
	public:
		friend class RemoteClient;

		~Server();

		static std::shared_ptr<Server> create(IoContext& ioContext);

		void connect(int port);

		void setDisconnectHandler(ServerDisconnectHandler&& disconnectHandler);

		void disconnect();

		void setConnectHandler(ServerConnectHandler&& acceptionFunction);

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
		Server(IoContext& ioContext);

		void removeClient(const RemoteClientPtr& client);

		void doAccept();

		void disconnectError(Error error);
		void disconnectError(const asio::system_error& e);

		asio::io_context& ioContext_;
		ServerConnectHandler connectHandler_;
		ServerDisconnectHandler disconnectHandler_;
		asio::ip::tcp::socket socket_;
		asio::ip::tcp::acceptor acceptor_;
		std::vector<RemoteClientPtr> clients_;

		std::atomic_bool active_ = false;
		std::atomic_bool allowConnections_ = false;
		bool closeConnection_ = false;
		asio::ip::port_type port_ = 0;
	};

}

#endif
