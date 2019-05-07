#ifndef CPPPROTONETWORK_NET_BUFFER_H
#define CPPPROTONETWORK_NET_BUFFER_H

#include "protobufmessage.h"

#include <mutex>
#include <queue>

namespace net {

	template <class T>
	class BufferQueue {
	public:
		BufferQueue() = default;

		void release(T&& message) {
			std::lock_guard<std::mutex> lock(mutex_);
			buffer_.push(std::move(message));
		}

		void acquire(T& message) {
			std::lock_guard<std::mutex> lock(mutex_);
			if (buffer_.empty()) {
				//message = T();
				return;
			}
			message = std::move(buffer_.front());
			buffer_.pop();
		}

		void clear() {
			while (!buffer_.empty()) {
				buffer_.pop();
			}
		}
		
		size_t getSize() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return buffer_.size();
		}

	private:
		std::mutex mutex_;
		std::queue<T> buffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_BUFFER_H
