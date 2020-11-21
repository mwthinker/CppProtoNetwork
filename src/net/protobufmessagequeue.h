#ifndef CPPPROTONETWORK_NET_PROTOBUFMESSAGEQUEUE_H
#define CPPPROTONETWORK_NET_PROTOBUFMESSAGEQUEUE_H

#include "protobufmessage.h"

#include <mutex>
#include <queue>

namespace net {
	
	class ProtobufMessageQueue {
	public:
		ProtobufMessageQueue() = default;

		explicit ProtobufMessageQueue(int messageSize);

		ProtobufMessageQueue(const ProtobufMessageQueue&) = delete;
		ProtobufMessageQueue(ProtobufMessageQueue&&) = delete;
		ProtobufMessageQueue& operator=(ProtobufMessageQueue&&) = delete;

		void release(ProtobufMessage&& message);

		void acquire(ProtobufMessage& message);

		void clear();
		
		int getSize() const;

		int getMessageSize() const;

		void setMessageSize(int messageSize);

	private:
		static constexpr int DEFAULT_MESSAGE_SIZE{512};

		mutable std::mutex mutex_;
		std::queue<ProtobufMessage> buffer_;
		int messageSize_{DEFAULT_MESSAGE_SIZE};
	};

}

#endif
