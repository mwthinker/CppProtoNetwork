#ifndef CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H
#define CPPPROTONETWORK_NET_NET_PROTOBUFMESSAGE_H

#include <vector>
#include <google/protobuf/message_lite.h>

namespace net {

	class ProtobufMessage {
	public:
		ProtobufMessage();

		explicit ProtobufMessage(int size);

		ProtobufMessage(const ProtobufMessage&) = default;

		ProtobufMessage(ProtobufMessage&&) noexcept;

		ProtobufMessage& operator=(const ProtobufMessage&) = default;

		ProtobufMessage& operator=(ProtobufMessage&&) noexcept;

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

	private:
		void reserveHeaderSize();

		void defineBodySize();

		std::vector<char> buffer_;
	};

}

#endif
