#ifndef CPPPROTONETWORK_NET_CLIENT_H
#define CPPPROTONETWORK_NET_CLIENT_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>
#include <thread>

namespace net {

	using ConnectHandler = std::function<void(std::error_code ec)>;

	class Client : public std::enable_shared_from_this<Client> {
	public:
		~Client();

		static std::shared_ptr<Client> create();

		void connect(const std::string& ip, int port);

		void send(const google::protobuf::MessageLite& message);

		void disconnect();

		void setConnectHandler(const ConnectHandler& connectHandler);

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler);

		bool isActive() const {
			return active_;
		}
		
		template <class Message>
		void setReceiveHandler(const ReceiveHandler<Message>& receiveHandler) {
			if (!active_) {
				connection_.setReceiveHandler<Message>(receiveHandler);
			}
		}

	private:
		Client();

		void close();

		asio::io_service ioService_;
		Connection connection_;
		ConnectHandler connectHandler_;
		std::atomic<bool> active_;
		std::thread thread_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CLIENT_H
