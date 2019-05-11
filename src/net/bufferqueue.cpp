#include "bufferqueue.h"

namespace {

	const size_t DEFAULT_MESSAGE_SIZE = 512;

}

namespace net {
	
	ProtobufMessageQueue::ProtobufMessageQueue() : messageSize_(DEFAULT_MESSAGE_SIZE) {
	}

	ProtobufMessageQueue::ProtobufMessageQueue(size_t messageSize) : messageSize_(messageSize) {
	}

	void ProtobufMessageQueue::release(ProtobufMessage&& message) {
		std::lock_guard<std::mutex> lock(mutex_);
		message.clear();
		buffer_.push(std::move(message));
	}

	void ProtobufMessageQueue::acquire(ProtobufMessage& message) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (buffer_.empty()) {
			message = ProtobufMessage(messageSize_);
			return;
		}
		message = std::move(buffer_.front());
		buffer_.pop();
	}

	void ProtobufMessageQueue::clear() {
		std::lock_guard<std::mutex> lock(mutex_);
		while (!buffer_.empty()) {
			buffer_.pop();
		}
	}

	size_t ProtobufMessageQueue::getSize() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return buffer_.size();
	}

	size_t ProtobufMessageQueue::getMessageSize() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return messageSize_;
	}

	void ProtobufMessageQueue::setMessageSize(size_t messageSize) {
		std::lock_guard<std::mutex> lock(mutex_);
		messageSize_ = messageSize;
	}

} // Namespace net.
