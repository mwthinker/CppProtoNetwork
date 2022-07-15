#ifndef CPPPROTONETWORK_NET_CLIENT_H
#define CPPPROTONETWORK_NET_CLIENT_H

#include "protobufmessage.h"

#include "detail/connection.h"
#include "iocontext.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>

namespace net {

	using ConnectHandler = std::function<void(std::error_code ec)>;

	class Client : public std::enable_shared_from_this<Client> {
	public:
		~Client();

		static std::shared_ptr<Client> create(IoContext& ioContext);

		void connect(const std::string& ip, int port);

		void send(const google::protobuf::MessageLite& message);

		void disconnect();

		void setConnectHandler(ConnectHandler&& connectHandler);

		void setDisconnectHandler(DisconnectHandler&& disconnectHandler);

		bool isActive() const {
			return active_;
		}
		
		template <MessageLite Message>
		void setReceiveHandler(ReceiveHandler<Message>&& receiveHandler);

	private:
		explicit Client(IoContext& ioContext);

		void close();

		void connectError(Error error);
		
		asio::io_context& ioContext_;
		detail::Connection connection_;
		ConnectHandler connectHandler_;
		std::atomic_bool active_ = false;
		std::mutex mutex_;
	};

	template <MessageLite Message>
	void net::Client::setReceiveHandler(ReceiveHandler<Message>&& receiveHandler) {
		connection_.setReceiveHandler<Message>(std::move(receiveHandler));
	}

}

#endif
