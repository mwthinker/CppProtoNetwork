#ifndef CPPPROTONETWORK_NET_AUXILIARY_H
#define CPPPROTONETWORK_NET_AUXILIARY_H

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <concepts>
#include <memory>
#include <limits>

namespace net {

	enum class Error {
		None,
		MessageMaxSize,
		MessageIncorrectSize,
		ProtobufProtocolError,
		InvalidPort,
		AlreadyActive
	};

	struct ConnectionErrorCategory : std::error_category {
		const char* name() const noexcept override;
		std::string message(int ev) const override;
	};

	std::error_code make_error_code(Error e);

	struct Meta {
		asio::ip::udp::endpoint endpoint;
	};

	using DisconnectHandler = std::function<void(const std::error_code& ec)>;
	using ServerDisconnectHandler = std::function<void(const std::system_error& ec)>;

	template <typename Message>
	concept MessageLite = std::derived_from<Message, google::protobuf::MessageLite>;

	template <MessageLite Message>
	using ReceiveHandler = std::function<void(const Message& message, const std::error_code& ec)>;

	inline bool isValidPort(int port) noexcept {
		return port > 0 && port < std::numeric_limits<asio::ip::port_type>::max();
	}

}

#endif
