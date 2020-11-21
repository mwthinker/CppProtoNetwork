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
			case Error::MessageMaxSize:
				return "received message to big";
			case Error::MessageIncorrectSize:
				return "received message differs from received size";
			case Error::ProtobufProtocolError:
				return "protobuf protocol error";
			default:
				return "(unrecognized error)";
		}
	}

	std::error_code make_error_code(Error e)
	{
		return {static_cast<int>(e), connectionErrorCategory};
	}

}
