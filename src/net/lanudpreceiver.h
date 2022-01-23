#ifndef CPPPROTONETWORK_NET_LANUDPRECEIVER_H
#define CPPPROTONETWORK_NET_LANUDPRECEIVER_H

#include "protobufmessage.h"
#include "auxiliary.h"

#include "detail/connection.h"

namespace net {

	template <MessageLite Message>
	using LanReceiveHandler = std::function<void(const Meta& meta, const Message& message, std::error_code ec)>;

	class LanUdpReceiver {
	public:
		LanUdpReceiver(asio::io_context& ioContext, int maxSize = 1024);

		~LanUdpReceiver();

		void setDisconnectHandler(DisconnectHandler&& disconnectHandler);

		void connect(unsigned short port);

		template <MessageLite Message>
		void setReceiveHandler(LanReceiveHandler<Message>&& receiveHandler);

		void disconnect();

		bool isActive() const;

	private:
		using InternalReceiveHandler = std::function<void(const Meta& meta, const ProtobufMessage& protobufMessage, std::error_code ec)>;

		void asyncReceive();

		void callReceivHandler(const Meta& meta, const ProtobufMessage& protobufMessage, const std::error_code& ec) const;

		void callHandle(const std::error_code& ec = {});

		asio::io_context& ioContext_;
		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		InternalReceiveHandler receiveHandler_;
		ProtobufMessage recvBuffer_;
		DisconnectHandler disconnectHandler_;
		int maxSize_ = 0;
		bool active_ = 0;
	};

	template <MessageLite Message>
	void LanUdpReceiver::setReceiveHandler(LanReceiveHandler<Message>&& receiveHandler) {
		receiveHandler_ = [protocolMessage = Message{}, messageHandler = std::move(receiveHandler)]
		(const Meta& meta, const ProtobufMessage& protobufMessage, std::error_code ec) mutable {
			protocolMessage.Clear();
			if (ec) {
				messageHandler(meta, protocolMessage, ec);
			} else {
				bool valid = protocolMessage.ParseFromArray(protobufMessage.getBodyData(), protobufMessage.getBodySize());
				if (valid) {
					messageHandler(meta, protocolMessage, ec);
				} else {
					messageHandler(meta, protocolMessage, make_error_code(Error::ProtobufProtocolError));
				}
			}
		};
	}

}

#endif
