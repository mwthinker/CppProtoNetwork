#include "protobufmessage.h"

namespace net {

	namespace {
		
		constexpr int decodeMessageSize(unsigned char byte1, unsigned char byte2) {
			return byte1 * 256 + byte2;
		}

	}

	ProtobufMessage::ProtobufMessage() {
	}

	ProtobufMessage::ProtobufMessage(int size)
		: buffer_(size + getHeaderSize()) {

		defineBodySize();
	}

	void ProtobufMessage::clear() {
		buffer_.resize(getHeaderSize());
		defineBodySize();
	}

	void ProtobufMessage::setBuffer(const google::protobuf::MessageLite& message) {
		int size = static_cast<int>(message.ByteSizeLong());
		buffer_.resize(getHeaderSize() + size);
		message.SerializeToArray(buffer_.data() + getHeaderSize(), size);
		defineBodySize();
	}

	void ProtobufMessage::reserveBodySize() {
		buffer_.resize(getHeaderSize() + getBodySize());
	}

	void ProtobufMessage::reserveBodySize(int size) {
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
		int bodySize = static_cast<int>(buffer_.size()) - getHeaderSize();
		buffer_[0] = ((bodySize >> 8) & 0xFF);
		buffer_[1] = (bodySize & 0xFF);
	}

}
