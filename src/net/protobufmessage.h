#ifndef CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H
#define CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H

#include <google/protobuf/message_lite.h>

#include <vector>

namespace net {

	class ReceiveMessage {
	public:

	};

	class ProtobufMessage {
	public:
		ProtobufMessage();

		explicit ProtobufMessage(int size);

		void clear();

		void setBuffer(const google::protobuf::MessageLite& message);

		int getSize() const noexcept {
			return static_cast<int>(buffer_.size());
		}

		constexpr int getHeaderSize() const noexcept {
			return 2;
		}

		void reserveBodySize();

		void reserveBodySize(int size);

		int getBodySize() const;

		unsigned const char* getData() const {
			return buffer_.data();
		}

		unsigned char* getData() {
			return buffer_.data();
		}

		unsigned const char* getBodyData() const {
			return buffer_.data() + getHeaderSize();
		}

		unsigned char* getBodyData() {
			return buffer_.data() + getHeaderSize();
		}

	private:
		void reserveHeaderSize();

		void defineBodySize();

		std::vector<unsigned char> buffer_;
	};

}

#endif
