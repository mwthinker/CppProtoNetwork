#ifndef CPPPROTONETWORK_NET_LANUDPSENDER_H
#define CPPPROTONETWORK_NET_LANUDPSENDER_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>
#include <thread>

namespace net {

	class LanUdpSender {
	public:
		~LanUdpSender() {
		}

		LanUdpSender(asio::io_service& ioService, size_t maxSize = 1024) : socket_(ioService),
			protobufMessage_(maxSize), maxSize_(maxSize), timer_(ioService),
			active_(false), duration_(std::chrono::seconds(5)) {
		}

		void disconnect() {
			if (active_) {
				std::lock_guard<std::mutex> lock(mutex_);
				timer_.expires_from_now();
				socket_.close();
				active_ = false;
			}
		}

		bool isActive() const {
			return active_;
		}

		void setMessage(const google::protobuf::MessageLite& message) {
			std::lock_guard<std::mutex> lock(mutex_);
			protobufMessage_.setBuffer(message);
		}

		std::error_code connect(unsigned short port) {
			if (active_) {
				return std::error_code();
			}

			std::lock_guard<std::mutex> lock(mutex_);
			remoteEndpoint_ = {asio::ip::address_v4::broadcast(), port};

			std::error_code ec;
			socket_.open(remoteEndpoint_.protocol(), ec);
			if (ec) {
				return ec;
			}
			socket_.set_option(asio::socket_base::reuse_address(true), ec);
			if (ec) {
				return ec;
			}
			if (socket_.set_option(asio::socket_base::broadcast(true), ec)) {
				return ec;
			}

			active_ = true;
			timer_.expires_after(duration_);
			timer_.async_wait([&](std::system_error se) {
				broadCast(se);
			});
			return ec;
		}

	private:
		void broadCast(std::system_error se) {
			socket_.async_send_to(asio::buffer(protobufMessage_.getData(), protobufMessage_.getSize()), remoteEndpoint_,
				[this, se](std::error_code ec, std::size_t length) mutable {

					if (ec) {
						return;
					}

					if (active_) {
						std::lock_guard<std::mutex> lock(mutex_);
						timer_.expires_after(duration_);
						timer_.async_wait([&](std::system_error se) {
							broadCast(se);
						});
					}
			});
		}

		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		ProtobufMessage protobufMessage_;
		asio::steady_timer timer_;
		bool active_;
		mutable std::mutex mutex_;
		std::chrono::seconds duration_;
		size_t maxSize_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_LANUDPSENDER_H
