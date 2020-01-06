#include "auxiliary.h"

namespace net {

	namespace {

		const ConnectionErrorCategory connectionErrorCategory{};

	}

	const char* ConnectionErrorCategory::name() const noexcept
	{
		return "connection";
	}

	std::string ConnectionErrorCategory::message(int ev) const
	{
		switch (static_cast<Error>(ev)) {
			case Error::MESSAGE_MAX_SIZE:
				return "received message to big";
			case Error::MESSAGE_INCORRECT_SIZE:
				return "received message differs from received size";
			case Error::PROTOBUF_PROTOCOL_ERROR:
				return "protobuf protocol error";
			default:
				return "(unrecognized error)";
		}
	}

	std::error_code make_error_code(Error e)
	{
		return {static_cast<int>(e), connectionErrorCategory};
	}

} // Namespace net.
