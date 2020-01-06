#ifndef CPPPROTONETWORK_NET_CLIENT_H
#define CPPPROTONETWORK_NET_CLIENT_H

#include "protobufmessage.h"

#include "detail/connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>

namespace net {

	using ConnectHandler = std::function<void(std::error_code ec)>;

	class Client : public std::enable_shared_from_this<Client> {
	public:
		~Client();

		static std::shared_ptr<Client> create(asio::io_context& ioContext);

		void connect(const std::string& ip, unsigned short port);

		void send(const google::protobuf::MessageLite& message);

		void disconnect();

		void setConnectHandler(const ConnectHandler& connectHandler);

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler);

		bool isActive() const {
			return active_;
		}
		
		template <class Message>
		void setReceiveHandler(ReceiveHandler<Message>&& receiveHandler);

	private:
		Client(asio::io_context& ioContext_);

		void close();

		asio::io_context& ioContext_;
		detail::Connection connection_;
		ConnectHandler connectHandler_;
		std::atomic<bool> active_{false};
		std::mutex mutex_;
	};

} // Namespace net.

template <class Message>
void net::Client::setReceiveHandler(ReceiveHandler<Message>&& receiveHandler) {
	if (!active_) {
		connection_.setReceiveHandler<Message>(std::forward<ReceiveHandler<Message>>(receiveHandler));
	}
}

#endif // CPPPROTONETWORK_NET_CLIENT_H
