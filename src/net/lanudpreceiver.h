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
		~LanUdpReceiver() {
		}

		LanUdpReceiver(asio::io_service& ioService, size_t maxSize = 1024)
			: socket_(ioService), maxSize_(maxSize), recvBuffer_(maxSize), active_(false) {
			
		}

		std::error_code connect(unsigned short port) {
			if (active_) {
				return std::error_code();
			}
			
			std::lock_guard<std::mutex> lock(mutex_);
			remoteEndpoint_ = { asio::ip::address_v4::any(), port };

			std::error_code ec;
			socket_.open(remoteEndpoint_.protocol(), ec);
			if (ec) {
				return ec;
			}
			
			socket_.set_option(asio::socket_base::reuse_address(true), ec);
			if (ec) {
				return ec;
			}

			socket_.bind(remoteEndpoint_, ec);
			
			if (!ec) {
				active_ = true;
				asyncReceive();
			}

			return ec;
		}
		
		template <class Message>
		void setReceiveHandler(int port, LanReceiveHandler<Message>&& receiveHandler) {
			static_assert(std::is_base_of<google::protobuf::MessageLite, Message>::value,
				"template type must have google::protobuf::MessageLite as base class");

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

		void disconnect() {
			if (active_) {
				std::lock_guard<std::mutex> lock(mutex_);
				socket_.close();
				active_ = false;
			}
		}

		bool isActive() const {
			return active_;
		}

	private:
		using InternalReceiveHandler = std::function<void(const Meta& meta, const ProtobufMessage& protobufMessage, std::error_code ec)>;

		void asyncReceive() {
			if (active_) {
				recvBuffer_.reserveBodySize(maxSize_);
				socket_.async_receive_from(
					asio::buffer(recvBuffer_.getData(), recvBuffer_.getSize()), remoteEndpoint_,					
					[&](std::error_code ec, std::size_t bytesTransferred) {
						Meta meta { remoteEndpoint_ };

						if (active_) {
							recvBuffer_.reserveBodySize();
							if (bytesTransferred != recvBuffer_.getSize()) {
								recvBuffer_.clear();

								callReceivHandler(meta, recvBuffer_, make_error_code(Error::MESSAGE_INCORRECT_SIZE));
								asyncReceive();
								return;
							}
							callReceivHandler(meta, recvBuffer_, ec);
						}
						asyncReceive();
					});
			}
		}

		void callReceivHandler(const Meta& meta, const ProtobufMessage& protobufMessage, const std::error_code& ec) const {
			if (receiveHandler_) {
				receiveHandler_(meta, protobufMessage, ec);
			}
		}
		
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