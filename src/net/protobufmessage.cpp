#include "protobufmessage.h"

namespace net {

	namespace {
		
		constexpr int decodeMessageSize(char byte1, char byte2) {
			return byte1 * 256 + byte2;
		}

	}

	ProtobufMessage::ProtobufMessage() {
	}

	ProtobufMessage::ProtobufMessage(size_t size)
		: buffer_(size + getHeaderSize()) {

		defineBodySize();
	}

	ProtobufMessage::ProtobufMessage(ProtobufMessage&& other) noexcept
		: buffer_{std::move(other.buffer_)} {
	}

	ProtobufMessage& ProtobufMessage::operator=(ProtobufMessage&& other) noexcept {
		buffer_ = std::move(other.buffer_);
		return *this;
	}

	void ProtobufMessage::clear() {
		buffer_.resize(getHeaderSize());
		defineBodySize();
	}

	void ProtobufMessage::setBuffer(const google::protobuf::MessageLite& message) {
		int size = message.ByteSize();
		buffer_.resize(getHeaderSize() + size);
		message.SerializeToArray(buffer_.data() + getHeaderSize(), size);
		defineBodySize();
	}

	void ProtobufMessage::reserveBodySize() {
		buffer_.resize(getHeaderSize() + getBodySize());
	}

	void ProtobufMessage::reserveBodySize(size_t size) {
		buffer_.resize(getHeaderSize() + size);
		defineBodySize();
	}

	int ProtobufMessage::getBodySize() const {
		if (buffer_.empty()) {
			return 0;
		}
		int size = decodeMessageSize(buffer_[0], buffer_[1]);
		return decodeMessageSize(buffer_[0], buffer_[1]);
	}

	void ProtobufMessage::reserveHeaderSize() {
		buffer_.resize(getHeaderSize());
	}

	void ProtobufMessage::defineBodySize() {
		int bodySize = buffer_.size() - getHeaderSize();
		buffer_[0] = ((bodySize >> 8) & 0xFF);
		buffer_[1] = (bodySize & 0xFF);
	}

} // Namespace net.
