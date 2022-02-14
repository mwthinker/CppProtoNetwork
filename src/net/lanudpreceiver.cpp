#include "lanudpreceiver.h"

namespace net {

	LanUdpReceiver::LanUdpReceiver(IoContext& ioContext, int maxSize)
		: ioContext_{ioContext.ioContext_}
		, socket_{ioContext_}
		, maxSize_{maxSize}
		, recvBuffer_{maxSize} {

	}

	void LanUdpReceiver::setDisconnectHandler(DisconnectHandler&& disconnectHandler) {
		disconnectHandler_ = std::move(disconnectHandler);
	}

	void LanUdpReceiver::connect(int port) {
		asio::post(ioContext_, [&]() {
			if (active_) {
				callHandle(net::Error::AlreadyActive);
				return;
			}

			if (!isValidPort(port)) {
				callHandle(net::Error::InvalidPort);
				return;
			}

			remoteEndpoint_ = {asio::ip::address_v4::any(), static_cast<asio::ip::port_type>(port)};

			std::error_code ec;
			socket_.open(remoteEndpoint_.protocol(), ec);
			if (ec) {
				callHandle(ec);
				return;
			}

			socket_.set_option(asio::socket_base::reuse_address(true), ec);
			if (ec) {
				callHandle(ec);
				return;
			}

			socket_.bind(remoteEndpoint_, ec);

			if (ec) {
				callHandle(ec);
				return;
			}

			active_ = true;
			asyncReceive();
		});
	}

	void LanUdpReceiver::disconnect() {
		if (active_) {
			socket_.close();
			active_ = false;
		}
	}

	bool LanUdpReceiver::isActive() const {
		return active_;
	}

	void LanUdpReceiver::asyncReceive() {
		recvBuffer_.reserveBodySize(maxSize_);
		socket_.async_receive_from(
			asio::buffer(recvBuffer_.getData(), recvBuffer_.getSize()), remoteEndpoint_,
			[&](std::error_code ec, std::size_t bytesTransferred) {
				Meta meta{remoteEndpoint_};

				if (active_) {
					recvBuffer_.reserveBodySize();
					if (bytesTransferred != recvBuffer_.getSize()) {
						recvBuffer_.clear();

						callReceivHandler(meta, recvBuffer_, make_error_code(Error::MessageIncorrectSize));
						asyncReceive();
						return;
					}
					callReceivHandler(meta, recvBuffer_, ec);
				}
				asyncReceive();
			});
	}

	void LanUdpReceiver::callReceivHandler(const Meta& meta, const ProtobufMessage& protobufMessage, const std::error_code& ec) const {
		if (receiveHandler_) {
			receiveHandler_(meta, protobufMessage, ec);
		}
	}

	void LanUdpReceiver::callHandle(const std::error_code& ec) {
		if (disconnectHandler_) {
			disconnectHandler_(ec);
		}
	}

	void LanUdpReceiver::callHandle(Error error) {
		callHandle(make_error_code(net::Error::InvalidPort));
	}

}
