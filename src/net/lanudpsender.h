#ifndef CPPPROTONETWORK_NET_LANUDPSENDER_H
#define CPPPROTONETWORK_NET_LANUDPSENDER_H

#include "protobufmessage.h"
#include "auxiliary.h"

#include "detail/connection.h"
#include "iocontext.h"

#include <functional>

namespace net {	

	class LanUdpSender {
	public:
		LanUdpSender(IoContext& ioContext, int maxSize = 1024);

		~LanUdpSender();
		
		void setDisconnectHandler(DisconnectHandler disconnectHandler);

		void disconnect();

		bool isActive() const;

		void setMessage(const google::protobuf::MessageLite& message);

		void connect(int port);

	private:
		void broadCast(std::system_error se);

		void callHandle(const std::error_code& ec = {});

		void callHandle(Error error);

		asio::io_context& ioContext_;
		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		ProtobufMessage protobufMessage_;
		DisconnectHandler disconnectHandler_;
		asio::steady_timer timer_;
		std::chrono::seconds duration_{1};
		int maxSize_ = 0;
		bool active_ = 0;
	};

}

#endif
