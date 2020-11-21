#include "protobufmessagequeue.h"

namespace net {

	ProtobufMessageQueue::ProtobufMessageQueue(int messageSize)
		: messageSize_{messageSize} {
	}

	void ProtobufMessageQueue::release(ProtobufMessage&& message) {
		std::lock_guard<std::mutex> lock{mutex_};
		message.clear();
		buffer_.push(std::move(message));
	}

	void ProtobufMessageQueue::acquire(ProtobufMessage& message) {
		std::lock_guard<std::mutex> lock{mutex_};
		if (buffer_.empty()) {
			message = ProtobufMessage{messageSize_};
			return;
		}
		message = std::move(buffer_.front());
		buffer_.pop();
	}

	void ProtobufMessageQueue::clear() {
		std::lock_guard<std::mutex> lock{mutex_};
		while (!buffer_.empty()) {
			buffer_.pop();
		}
	}

	int ProtobufMessageQueue::getSize() const {
		std::lock_guard<std::mutex> lock{mutex_};
		return static_cast<int>(buffer_.size());
	}

	int ProtobufMessageQueue::getMessageSize() const {
		std::lock_guard<std::mutex> lock{mutex_};
		return messageSize_;
	}

	void ProtobufMessageQueue::setMessageSize(int messageSize) {
		std::lock_guard<std::mutex> lock{mutex_};
		messageSize_ = messageSize;
	}

}
