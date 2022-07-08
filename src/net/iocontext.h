#ifndef CPPPROTONETWORK_NET_IOCONTEXT_H
#define CPPPROTONETWORK_NET_IOCONTEXT_H

#include <asio/io_context.hpp>

namespace net {

	class IoContext {
	public:
		IoContext() = default;

		friend class Server;
		friend class Client;
		friend class RemoteClient;
		friend class Timer;
		friend class LanUdpSender;
		friend class LanUdpReceiver;

		void run();

		void runOne();

	private:
		asio::io_context ioContext_;
	};

}

#endif
