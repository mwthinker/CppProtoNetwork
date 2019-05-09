#ifndef CPPPROTONETWORK_NET_CLIENT_H
#define CPPPROTONETWORK_NET_CLIENT_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <thread>

namespace net {

	template <class T>
	class IClient : public std::enable_shared_from_this<IClient<T>> {
	public:
		using DisconnectHandler = std::function<void(std::error_code ec)>;

		using ReceiveHandler = std::function<void(MessageLitePtr&& message, std::error_code ec)>;

		virtual void setReceiveHandler(const ReceiveHandler& messageHandler) = 0;

		virtual void setDisconnectHandler(const DisconnectHandler& disconnectHandler) = 0;

		virtual void connect(const std::string& ip, int port) = 0;

		virtual void send(const google::protobuf::MessageLite& message) = 0;
	};

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
		
		void setReceiveHandler(const ReceiveHandler& receiveHandler) {
			connection_.setReceiveHandler(receiveHandler);
		}

		void release(ProtobufMessage&& message) {
			connection_.release(std::move(message));
		}

	private:
		Client();

		asio::io_service ioService_;
		Connection connection_;
		ConnectHandler connectHandler_;
		std::thread thread_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CLIENT_H
