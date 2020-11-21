#ifndef CPPPROTONETWORK_NET_AUXILIARY_H
#define CPPPROTONETWORK_NET_AUXILIARY_H

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

namespace net {

	enum class Error {
		None,
		MessageMaxSize,
		MessageIncorrectSize,
		ProtobufProtocolError
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

	template <typename Message>
	using ReceiveHandler = std::function<void(const Message& message, std::error_code ec)>;

	template <typename Message>
	constexpr void staticAssertBaseOfMessageLite() {
		static_assert(std::is_base_of<google::protobuf::MessageLite, Message>::value,
			"template type must have google::protobuf::MessageLite as base class");
	}

}

#endif
