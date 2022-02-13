#include "timer.h"

namespace net {

	Timer::Timer(IoContext& ioContext)
		: timer_{ioContext.ioContext_} {
	}

	void Timer::expiresAfter(const std::chrono::steady_clock::duration& duration) {
		timer_.expires_after(duration);
	}

}
