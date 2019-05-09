#ifndef CPPPROTONETWORK_NET_BUFFER_H
#define CPPPROTONETWORK_NET_BUFFER_H

#include "protobufmessage.h"

#include <mutex>
#include <queue>

namespace net {
	
	class ProtobufMessageQueue {
	public:
		ProtobufMessageQueue() : messageSize_(DEFAULT_MESSAGE_SIZE) {
		}

		ProtobufMessageQueue(size_t messageSize) : messageSize_(messageSize) {
		}

		ProtobufMessageQueue(const ProtobufMessageQueue&) = delete;
		ProtobufMessageQueue(ProtobufMessageQueue&&) = delete;
		ProtobufMessageQueue& operator=(ProtobufMessageQueue&&) = delete;

		void release(ProtobufMessage&& message) {
			std::lock_guard<std::mutex> lock(mutex_);
			message.clear();
			buffer_.push(std::move(message));
		}

		void acquire(ProtobufMessage& message) {
			std::lock_guard<std::mutex> lock(mutex_);
			if (buffer_.empty()) {
				message = ProtobufMessage(messageSize_);
				return;
			}
			message = std::move(buffer_.front());
			buffer_.pop();
		}

		void clear() {
			std::lock_guard<std::mutex> lock(mutex_);
			while (!buffer_.empty()) {
				buffer_.pop();
			}
		}
		
		size_t getSize() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return buffer_.size();
		}

		size_t getMessageSize() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return messageSize_;
		}

		void setMessageSize(size_t messageSize) {
			std::lock_guard<std::mutex> lock(mutex_);
			messageSize_ = messageSize;
		}

	private:
		static const size_t DEFAULT_MESSAGE_SIZE = 512;

		mutable std::mutex mutex_;
		size_t messageSize_;
		std::queue<ProtobufMessage> buffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_BUFFER_H
