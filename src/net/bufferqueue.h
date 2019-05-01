#ifndef CPPPROTONETWORK_NET_BUFFER_H
#define CPPPROTONETWORK_NET_BUFFER_H

#include "protobufmessage.h"

#include <mutex>
#include <queue>

namespace net {

	class BufferQueue {
	public:
		BufferQueue() = default;

		void release(net::ProtobufMessage&& message);

		void acquire(net::ProtobufMessage& message);

		void clear();

	private:
		std::mutex mutex_;
		std::queue<net::ProtobufMessage> buffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_BUFFER_H
