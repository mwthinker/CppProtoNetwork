#ifndef CPPPROTONETWORK_NET_LOGGER_H
#define CPPPROTONETWORK_NET_LOGGER_H

#include <spdlog/spdlog.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> 

namespace net {
	
	static std::shared_ptr<spdlog::logger> logger(const char* name = "net") {
		auto logger = spdlog::get(name);
		if (logger == nullptr) {
			logger = spdlog::stdout_color_mt(name);
		}
		return logger;
	}

} // Namespace net.

#endif // CPPPROTONETWORK_NET_LOGGER_H
