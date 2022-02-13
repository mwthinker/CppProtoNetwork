#ifndef CPPPROTONETWORK_NET_TIMER_H
#define CPPPROTONETWORK_NET_TIMER_H

#include "iocontext.h"

#include <asio/io_context.hpp>

#include <asio/steady_timer.hpp>

namespace net {

	class Timer {
	public:
		explicit Timer(IoContext& ioContext);

		void expiresAfter(const std::chrono::steady_clock::duration& duration);

		template <typename WaitHandler>
		void asyncWait(WaitHandler&& waitHandler) {
			timer_.async_wait(waitHandler);
		}

	private:
		asio::steady_timer timer_;
	};

}

#endif
