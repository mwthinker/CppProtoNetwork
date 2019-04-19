#include "buffer.h"

#include <cassert>

namespace net {

	int Buffer::lastId_ = 0;

	Buffer::Buffer(const std::shared_ptr<std::mutex>& mutex) : mutex_(mutex),
		active_(true), id_(++lastId_) {

	}

	bool Buffer::popReceiveBuffer(Packet& packet) {
		mutex_->lock();
		if (!receiveBuffer_.empty()) {
			unsigned int size = receiveBuffer_[0];
			if (receiveBuffer_.size() >= size) {
				packet = net::Packet(receiveBuffer_.data(), size);
				receiveBuffer_.erase(receiveBuffer_.begin(), receiveBuffer_.begin() + size);
				mutex_->unlock();
				return true;
			}
		}
		mutex_->unlock();
		return false;
	}

	void Buffer::addToSendBuffer(const Packet& packet) {
		mutex_->lock();
		sendBuffer_.insert(sendBuffer_.end(), packet.getData(), packet.getData() + packet.getSize());
		mutex_->unlock();
	}

	void Buffer::addToReceiveBuffer(char data) {
		mutex_->lock();
		receiveBuffer_.push_back(data);
		mutex_->unlock();
	}

	bool Buffer::popSendBuffer(Packet& packet) {
		mutex_->lock();
		if (!sendBuffer_.empty()) {
			unsigned int size = sendBuffer_[0];
			assert(size <= Packet::MAX_SIZE);
			if (sendBuffer_.size() >= size) {
				packet = Packet(sendBuffer_.data(), size);
				sendBuffer_.erase(sendBuffer_.begin(), sendBuffer_.begin() + size);
				mutex_->unlock();
				return true;
			}
		}
		mutex_->unlock();
		return false;
	}

	bool Buffer::isActive() const {
		mutex_->lock();
		bool active = active_;
		mutex_->unlock();
		return active;
	}

	void Buffer::setActive(bool active) {
		mutex_->lock();
		active_ = active;
		mutex_->unlock();
	}

} // Namespace net.
