#ifndef CPPPROTONETWORK_NET_BUFFERQUEUE_H
#define CPPPROTONETWORK_NET_BUFFERQUEUE_H

#include "protobufmessage.h"

#include <mutex>
#include <queue>

namespace net {
	
	class ProtobufMessageQueue {
	public:
		ProtobufMessageQueue() = default;

		explicit ProtobufMessageQueue(size_t messageSize);

		ProtobufMessageQueue(const ProtobufMessageQueue&) = delete;
		ProtobufMessageQueue(ProtobufMessageQueue&&) = delete;
		ProtobufMessageQueue& operator=(ProtobufMessageQueue&&) = delete;

		void release(ProtobufMessage&& message);

		void acquire(ProtobufMessage& message);

		void clear();
		
		size_t getSize() const;

		size_t getMessageSize() const;

		void setMessageSize(size_t messageSize);

	private:
		static constexpr size_t DEFAULT_MESSAGE_SIZE{512};

		mutable std::mutex mutex_;
		std::queue<ProtobufMessage> buffer_;
		size_t messageSize_{DEFAULT_MESSAGE_SIZE};
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_BUFFERQUEUE_H
