#include "bufferqueue.h"
#include "protobufmessage.h"

namespace net {	
	
	void BufferQueue::release(net::ProtobufMessage&& message) {
		std::lock_guard<std::mutex> lock(mutex_);
		buffer_.push(std::move(message));
	}

	void BufferQueue::acquire(net::ProtobufMessage& message) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (buffer_.empty()) {
			return;
		}

		message = std::move(buffer_.front());
		buffer_.pop();
	}

	void BufferQueue::clear() {
		while (!buffer_.empty()) {
			buffer_.pop();
		}
	}

} // Namespace net.
