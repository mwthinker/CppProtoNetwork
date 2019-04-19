#ifndef NET_PACKET_H
#define NET_PACKET_H

#include <array>
#include <algorithm>
#include <cassert>

namespace net {

	// The class Packet represent a holder of formatted data. No dynamic data, is used.
	class Packet {
	public:
		static const int MAX_SIZE = 128;

		// Creates an empty packet with size ONE.
		// The one extra byte is used to store the size.
		// E.g:
		// Packet packet;
		// std::cout << "Example, size of empty packet: " << packet.getSize() << "." << std::endl;
		//
		// Run the example:
		// >> Example, size of empty packet: 1.
		Packet() : index_(1) {
			data_[0] = 1;
		}

		// Fill an empty packet with the data.
		// The first byte must represent the size of the packet.
		Packet(const char* data, int size) : index_(1) {
			assert(size > 0 &&  size <= Packet::MAX_SIZE);
			assert(data[0] == size);
			std::copy(data, data + size, data_.data());
		}

		Packet& operator>>(char& byte) {
			byte = data_[index_++];
			return *this;
		}

		Packet& operator<<(char byte) {
			pushBack(byte);
			return *this;
		}

		const char* getData() const {
			return data_.data();
		}

		int getSize() const {
			return data_[0];
		}

		void pushBack(char byte) {
			data_[data_[0]++] = byte;
		}

		char& operator[](int index) {
			return data_[index];
		}

		char operator[](int index) const {
			return data_[index];
		}

		unsigned int dataLeftToRead() const {
			return data_[0] - index_;
		}
		
		void reset() {
			index_ = 1;
		}

		void clear() {
			index_ = 1;
			data_[0] = 1;
		}

	private:
		std::array<char, MAX_SIZE> data_;
		int index_; // The index for the next byte to be read.
	};

} // Namespace net.

#endif // NET_PACKET_H
