#ifndef CPPPROTONETWORK_NET_LANUDPRECEIVER_H
#define CPPPROTONETWORK_NET_LANUDPRECEIVER_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>
#include <thread>

namespace net {

	template <class Message>
	using LanReceiveHandler = std::function<void(const Meta& meta, const Message& message, std::error_code ec)>;	

	class LanUdpReceiver {
	public:
		~LanUdpReceiver();

		LanUdpReceiver(asio::io_service& ioService, size_t maxSize = 1024);

		std::error_code connect(unsigned short port);
		
		template <class Message>
		void setReceiveHandler(int port, LanReceiveHandler<Message>&& receiveHandler) {
			IS_BASE_OF_MESSAGELITE<Message>();

			std::lock_guard<std::mutex> lock(mutex_);
			Message protocolMessage;
			receiveHandler_ = [protocolMessage, messageHandler = std::forward<LanReceiveHandler<Message>>(receiveHandler)]
			(const Meta& meta, const ProtobufMessage& protobufMessage, std::error_code ec) mutable {
				protocolMessage.Clear();
				if (ec) {
					messageHandler(meta, protocolMessage, ec);
				} else {
					bool valid = protocolMessage.ParseFromArray(protobufMessage.getBodyData(), protobufMessage.getBodySize());
					if (valid) {
						messageHandler(meta, protocolMessage, ec);
					} else {
						messageHandler(meta, protocolMessage, make_error_code(Error::PROTOBUF_PROTOCOL_ERROR));
					}
				}
			};
		}

		void disconnect();

		bool isActive() const;

	private:
		using InternalReceiveHandler = std::function<void(const Meta& meta, const ProtobufMessage& protobufMessage, std::error_code ec)>;

		void asyncReceive();

		void callReceivHandler(const Meta& meta, const ProtobufMessage& protobufMessage, const std::error_code& ec) const;
		
		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		InternalReceiveHandler receiveHandler_;
		
		mutable std::mutex mutex_;
		ProtobufMessage recvBuffer_;
		size_t maxSize_;
		bool active_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_LANUDPRECEIVER_H
