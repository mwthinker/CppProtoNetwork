#ifndef NET_NETWORK_H
#define NET_NETWORK_H

#include "packet.h"
#include "buffer.h"

#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <thread>

#include <asio.hpp>

namespace net {

    class Server;
    class Client;
    class Connection;
	class ConnectionsControl; 

	enum Status {
		ACTIVE, NOT_ACTIVE
	};

	/// This class works as a multi-user system. Should be used
	/// to control a server/client system. Uses an internal thread which makes
	/// all functions to be non blocking and safe to use.
	class Network {
	public:
		/// Create a network object.
		/// @param sleepMilliseconds the sleeping time in milliseconds for the internal thread cycle.
		Network(int sleepMilliseconds = 0);

		/// The destructor is the same as calling the stop function.
		~Network();

		/// Is not copyable.
		Network(const Network&) = delete;

		/// Is not copyable.
		Network& operator=(const Network&) = delete;

		/// Start the server. Must have status NOT_ACTIVE otherwise nothing will happen.
		/// The port must be available and not blocked by the firewall.
		/// @param port the active port number for the socket to receive connections from.
		void startServer(int port);

		/// Start the client. Must have status NOT_ACTIVE otherwise nothing will happen.
		/// The ip and port to the server to be connecting to.
		/// @param serverIp the ip number for for the server to connect to. E.g. @param serverIp= "1.1.1.1".
		/// @param port the port number for for the server to connect to.
		void startClient(std::string serverIp, int port);

		/// End all active connections.
		/// Stops the internal thread and the status will change to NOT_ACTIVE.
		/// Blocks until the thread is closed, should be pretty fast.
		/// May call start again, as server or client, you choose.
		void stop();

		/// Set the server to connect connections, is true by default.
		/// Does nothing if the status is NOT_ACTIVE and/or in client mode.
		void setAcceptConnections(bool accept);

		/// @return if the network works as a server and accepts new connections.
		bool isAcceptingConnections() const;

		/// Polls the latest connections made.
		/// @return The latest connection, if there are no new connections null is returned.
		std::shared_ptr<Connection> pollConnection();

		/// Give the current status for the network object.
		/// @return The current status.
		Status getStatus() const;

		/// Give the information if the network is an active server.
		/// @return True if the network is an active server else false.
		bool isServer() {
			return isServer_;
		}

		/// Give the information if the network is an active client.
		/// @return True if the network is an active client else false.
		bool isClient() {
			return isClient_;
		}

	private:
		asio::io_service service_;
		ConnectionsControl* connectionsControl_;

		std::shared_ptr<std::mutex> mutex_;
		std::thread thread_;
		int sleepMilliseconds_;

		Status status_;
		bool isServer_;
		bool isClient_;
		static bool firstInstance;
	};

} // Namespace net.

#endif // NET_NETWORK_H
