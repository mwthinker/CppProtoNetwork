#ifndef CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H
#define CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H

#include <vector>
#include <google/protobuf/message_lite.h>

namespace net {

	class ProtobufMessage {
	public:
		ProtobufMessage() : ProtobufMessage(0) {
		}

		ProtobufMessage(size_t size) : buffer_(size + getHeaderSize()) {
			defineBodySize();
		}

		ProtobufMessage(const ProtobufMessage&) = default;

		ProtobufMessage(ProtobufMessage&& other) noexcept : buffer_(std::move(other.buffer_)) {
		}

		ProtobufMessage& operator=(const ProtobufMessage&) = default;

		ProtobufMessage& operator=(ProtobufMessage&& other) noexcept {
			buffer_ = std::move(other.buffer_);
			return *this;
		}

		void clear() {
			buffer_.resize(getHeaderSize());
			defineBodySize();
		}

		void setBuffer(const google::protobuf::MessageLite& message) {
			int size = message.ByteSize();
			buffer_.resize(getHeaderSize() + size);
			message.SerializeToArray(buffer_.data() + getHeaderSize(), size);
			defineBodySize();
		}

		size_t getSize() const noexcept {
			return buffer_.size();
		}

		constexpr int getHeaderSize() const {
			return 2;
		}

		void reserveHeaderSize() {
			buffer_.resize(getHeaderSize());
		}

		void reserveBodySize() {
			buffer_.resize(getHeaderSize() + getBodySize());
		}

		int getBodySize() const {
			if (buffer_.empty()) {
				return 0;
			}
			int size = decodeMessageSize(buffer_[0], buffer_[1]);
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

		static constexpr int decodeMessageSize(char byte1, char byte2) {
			return byte1 * 256 + byte2;
		}

	private:
		void defineBodySize() {
			int bodySize = buffer_.size() - getHeaderSize();
			buffer_[0] = ((bodySize >> 8) & 0xFF);
			buffer_[1] = (bodySize & 0xFF);
		}

		std::vector<char> buffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_PROTOBUFMESSAGE_H
