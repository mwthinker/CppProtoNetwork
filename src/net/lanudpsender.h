#ifndef CPPPROTONETWORK_NET_LANUDPSENDER_H
#define CPPPROTONETWORK_NET_LANUDPSENDER_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>
#include <thread>

namespace net {

	class LanUdpSender {
	public:
		~LanUdpSender();

		LanUdpSender(asio::io_context& ioContext, size_t maxSize = 1024);

		void disconnect();

		bool isActive() const;

		void setMessage(const google::protobuf::MessageLite& message);

		std::error_code connect(unsigned short port);

	private:
		void broadCast(std::system_error se);

		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		ProtobufMessage protobufMessage_;
		asio::steady_timer timer_;
		mutable std::mutex mutex_;
		std::chrono::seconds duration_{1};
		size_t maxSize_{1024};
		bool active_{false};
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_LANUDPSENDER_H
