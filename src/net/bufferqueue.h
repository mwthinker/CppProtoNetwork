#ifndef CPPPROTONETWORK_NET_BUFFER_H
#define CPPPROTONETWORK_NET_BUFFER_H

#include "protobufmessage.h"

#include <mutex>
#include <queue>

namespace net {
	
	class ProtobufMessageQueue {
	public:
		ProtobufMessageQueue();

		ProtobufMessageQueue(size_t messageSize);

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
		mutable std::mutex mutex_;
		size_t messageSize_;
		std::queue<ProtobufMessage> buffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_BUFFER_H
