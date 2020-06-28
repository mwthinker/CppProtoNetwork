#ifndef CPPPROTONETWORK_NET_LANUDPSENDER_H
#define CPPPROTONETWORK_NET_LANUDPSENDER_H

#include "protobufmessage.h"
#include "auxiliary.h"

#include "detail/connection.h"

#include <functional>

namespace net {	

	class LanUdpSender {
	public:
		LanUdpSender(asio::io_context& ioContext, size_t maxSize = 1024);

		~LanUdpSender();
		
		template <class Handler>
		void setDisconnectHandler(Handler&& disconnectHandler) {
			disconnectHandler_ = disconnectHandler;
		}

		void disconnect();

		bool isActive() const;

		void setMessage(const google::protobuf::MessageLite& message);

		void connect(unsigned short port);

	private:
		void broadCast(std::system_error se);

		void callHandle(const std::error_code& ec = {});

		asio::io_context& ioContext_;
		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		ProtobufMessage protobufMessage_;
		DisconnectHandler disconnectHandler_;
		asio::steady_timer timer_;
		std::chrono::seconds duration_{1};
		size_t maxSize_{};
		bool active_{};
	};

}

#endif
