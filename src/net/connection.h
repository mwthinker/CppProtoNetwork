#ifndef CPPPROTONETWORK_NET_CONNECTION_H
#define CPPPROTONETWORK_NET_CONNECTION_H

#include "protobufmessage.h"
#include "bufferqueue.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <deque>
#include <functional>
#include <mutex>
#include <queue>

namespace net {

	enum class Error {
		NONE,
		MESSAGE_MAX_SIZE,
		PROTOBUF_PROTOCOL_ERROR
	};

	struct ConnectionErrorCategory : std::error_category
	{
		const char* name() const noexcept override;
		std::string message(int ev) const override;
	};

	std::error_code make_error_code(Error e);

	using MessageLitePtr = std::unique_ptr<google::protobuf::MessageLite>;
	//using MessageQueuePtr = std::shared_ptr<BufferQueue<MessageLitePtr>>;

	using DisconnectHandler = std::function<void(std::error_code ec)>;
	
	using ReceiveHandler = std::function<void(ProtobufMessage&& message, std::error_code ec)>;

	template <class Derived>
	std::unique_ptr<Derived> castProtobufMessage(MessageLitePtr&& base) noexcept {
		static_assert(std::is_base_of<google::protobuf::MessageLite, Derived>::value,
			"template type must have google::protobuf::MessageLite as base class");
		return std::unique_ptr<Derived>(dynamic_cast<Derived*>(base.release()));
	}

	class Connection {
	public:
		Connection(asio::ip::tcp::socket socket);
		~Connection();

		void send(const google::protobuf::MessageLite& message);

		void setReceiveHandler(const ReceiveHandler& messageHandler);

		void setDisconnectHandler(const DisconnectHandler& disconnectHandler);

		void readHeader();

		void disconnect(std::error_code ec);

		void release(ProtobufMessage&&);

		asio::ip::tcp::socket& getSocket() {
			return socket_;
		}

	protected:
		asio::ip::tcp::socket socket_;

		using InternalReceiveHandler = ReceiveHandler;//std::function<void(const net::ProtobufMessage&& message, std::error_code ec)>;

		void readBody();

		InternalReceiveHandler receiveHandler_;
		DisconnectHandler disconnectHandler_;
		ProtobufMessageQueue sendBuffer_;
		ProtobufMessageQueue receiveBuffer_;
		net::ProtobufMessage receiveMessage_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_CONNECTION_H
