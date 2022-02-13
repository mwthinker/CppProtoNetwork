#include "iocontext.h"

namespace net {

	void IoContext::run() {
		ioContext_.run();
	}

	void IoContext::runOne() {
		ioContext_.run_one();
	}

}
