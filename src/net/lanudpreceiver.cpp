#include "lanudpreceiver.h"

namespace net {

	LanUdpReceiver::~LanUdpReceiver() {
	}

	LanUdpReceiver::LanUdpReceiver(asio::io_service& ioService, size_t maxSize)
		: socket_(ioService), maxSize_(maxSize), recvBuffer_(maxSize), active_(false) {

	}

	std::error_code LanUdpReceiver::connect(unsigned short port) {
		if (active_) {
			return std::error_code();
		}

		std::lock_guard<std::mutex> lock(mutex_);
		remoteEndpoint_ = {asio::ip::address_v4::any(), port};

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

	void LanUdpReceiver::disconnect() {
		if (active_) {
			std::lock_guard<std::mutex> lock(mutex_);
			socket_.close();
			active_ = false;
		}
	}

	bool LanUdpReceiver::isActive() const {
		return active_;
	}

	void LanUdpReceiver::asyncReceive() {
		if (active_) {
			recvBuffer_.reserveBodySize(maxSize_);
			socket_.async_receive_from(
				asio::buffer(recvBuffer_.getData(), recvBuffer_.getSize()), remoteEndpoint_,
				[&](std::error_code ec, std::size_t bytesTransferred) {
					Meta meta{remoteEndpoint_};

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

	void LanUdpReceiver::callReceivHandler(const Meta& meta, const ProtobufMessage& protobufMessage, const std::error_code& ec) const {
		if (receiveHandler_) {
			receiveHandler_(meta, protobufMessage, ec);
		}
	}

} // Namespace net.
