#ifndef CPPPROTONETWORK_NET_AUXILIARY_H
#define CPPPROTONETWORK_NET_AUXILIARY_H

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

namespace net {

	enum class Error {
		NONE,
		MESSAGE_MAX_SIZE,
		MESSAGE_INCORRECT_SIZE,
		PROTOBUF_PROTOCOL_ERROR
	};

	struct ConnectionErrorCategory : std::error_category {
		const char* name() const noexcept override;
		std::string message(int ev) const override;
	};

	std::error_code make_error_code(Error e);

	class Meta {
	public:
		asio::ip::udp::endpoint endpoint_;
	};

	using DisconnectHandler = std::function<void(std::error_code ec)>;
	using ServerDisconnectHandler = std::function<void(std::system_error ec)>;

	template <class Message>
	using ReceiveHandler = std::function<void(const Message & message, std::error_code ec)>;

	template <class Message>
	constexpr void IS_BASE_OF_MESSAGELITE() {
		static_assert(std::is_base_of<google::protobuf::MessageLite, Message>::value,
			"template type must have google::protobuf::MessageLite as base class");
	}

} // Namespace net.

#endif // CPPPROTONETWORK_NET_AUXILIARY_H
