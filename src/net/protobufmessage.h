#ifndef CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H
#define CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H

#include <vector>
#include <algorithm>
#include <string>

#include <google/protobuf/message_lite.h>

namespace net {

	class ProtobufMessage {
	public:
		ProtobufMessage() : buffer_(getHeaderSize()) {
			defineBodySize();
		}

		ProtobufMessage(const google::protobuf::MessageLite& message) : buffer_(getHeaderSize()) {
			int size = message.ByteSize();
			buffer_.resize(getHeaderSize() + size);
			message.SerializeToArray(buffer_.data() + getHeaderSize(), size);
			defineBodySize();
		}

		ProtobufMessage(ProtobufMessage&& other) noexcept {
		}

		ProtobufMessage& operator=(ProtobufMessage&& protobufMessage) noexcept {
			return *this;
		}

		void clear() {
			buffer_.resize(2);
			defineBodySize();
		}

		void setBuffer(const char* buffer, int size) {
			buffer_.resize(getHeaderSize() + size);
			buffer_.insert(buffer_.begin() + getHeaderSize(), buffer, buffer + size);
			defineBodySize();
		}

		size_t getSize() const {
			return buffer_.size();
		}

		static constexpr int getHeaderSize() {
			return 2;
		}

		void reserveBodySize() {
			buffer_.resize(getHeaderSize() + getBodySize());
		}

		int getBodySize() const {
			return decodeMessageSize(buffer_[0], buffer_[1]);
		}

		const char* getData() const {
			return buffer_.data();
		}

		char* getData() {
			return buffer_.data();
		}

		const char* getBodyData() const {
			return buffer_.data() + getHeaderSize();
		}

		char* getBodyData() {
			return buffer_.data() + getHeaderSize();
		}

		static int decodeMessageSize(char byte1, char byte2) {
			return byte1 * 256 + byte2;
		}

	private:
		void defineBodySize() {
			static_assert(getHeaderSize() == 2, "Header size is assumed to be 2");

			int bodySize = buffer_.size() - getHeaderSize();
			buffer_[0] = ((bodySize >> 8) & 0xFF);
			buffer_[1] = (bodySize & 0xFF);
		}

		std::vector<char> buffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_PROTOBUFMESSAGE_H
