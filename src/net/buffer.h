#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include "packet.h"

#include <mutex>
#include <vector>
#include <memory>

namespace net {

	class Buffer {
	public:
		Buffer(const std::shared_ptr<std::mutex>& mutex);

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		bool popReceiveBuffer(Packet& packet);

		void addToSendBuffer(const Packet& packet);


		void addToReceiveBuffer(char data);

		bool popSendBuffer(Packet& packet);

		
		bool isActive() const;

		void setActive(bool active);

		int getId() const {
			return id_;
		}

	private:
		std::vector<char> receiveBuffer_;
		std::vector<char> sendBuffer_;
		
		std::shared_ptr<std::mutex> mutex_;
		
		bool active_;
		int id_;
		static int lastId_;
	};

} // Namespace net.

#endif // NET_BUFFER_H
