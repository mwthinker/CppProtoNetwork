#include "lanudpsender.h"

using namespace std::chrono_literals;

namespace net {

	LanUdpSender::LanUdpSender(IoContext& ioContext, int maxSize)
		: ioContext_{ioContext.ioContext_}
		, socket_{ioContext_}
		, protobufMessage_{maxSize}
		, maxSize_{maxSize}
		, timer_{ioContext_} {
	}

	LanUdpSender::~LanUdpSender() {
		disconnect();
	}

	void LanUdpSender::setDisconnectHandler(DisconnectHandler disconnectHandler) {
		disconnectHandler_ = disconnectHandler;
	}

	void LanUdpSender::disconnect() {
		if (active_) {
			timer_.cancel();
			socket_.close();
			active_ = false;
			callHandle();
		}
	}

	bool LanUdpSender::isActive() const {
		return active_;
	}

	void LanUdpSender::setMessage(const google::protobuf::MessageLite& message) {
		protobufMessage_.setBuffer(message);
	}

	void LanUdpSender::connect(unsigned short port) {
		if (active_) {
			return;
		}
		asio::post(ioContext_, [&]() {
			remoteEndpoint_ = {asio::ip::address_v4::broadcast(), port};

			std::error_code ec;
			socket_.open(remoteEndpoint_.protocol(), ec);
			if (ec) {				
				callHandle(ec);
			}
			socket_.set_option(asio::socket_base::reuse_address{true}, ec);
			if (ec) {
				callHandle(ec);
			}
			socket_.set_option(asio::socket_base::broadcast{true}, ec);
			if (ec) {
				callHandle(ec);
			}

			active_ = true;
			timer_.expires_after(duration_);
			timer_.async_wait([&](std::system_error se) {
				broadCast(se);
			});
		});
	}

	void LanUdpSender::broadCast(std::system_error se) {
		socket_.async_send_to(asio::buffer(protobufMessage_.getData(), protobufMessage_.getSize()), remoteEndpoint_,
			[this, se](std::error_code ec, std::size_t length) mutable {

				if (ec) {
					callHandle(ec);
					return;
				}

				if (active_) {
					timer_.expires_after(duration_);
					timer_.async_wait([&](std::system_error se) {
						broadCast(se);
					});
				}
			});
	}

	void LanUdpSender::callHandle(const std::error_code& ec) {
		if (disconnectHandler_) {
			disconnectHandler_(ec);
		}
	}

}
