#ifndef CPPPROTONETWORK_NET_CLIENT_H
#define CPPPROTONETWORK_NET_CLIENT_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

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

		void setConnectHandler(const ConnectHandler& connectHandler) {
			connectHandler_ = connectHandler;
		}

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler) {
			connection_.setDisconnectHandler(disconnectHandler);
		}

		template <typename ProtocolMessage>
		void setReceiveHandler(std::function<void(const ProtocolMessage&)> messageHandler) {
			connection_.setReceiveHandler(messageHandler);
		}

	private:
		Client();

		asio::io_service ioService_;
		Connection connection_;
		ConnectHandler connectHandler_;
		std::thread thread_;
		bool active_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CLIENT_H
