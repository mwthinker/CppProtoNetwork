#include "connection.h"

#include <asio.hpp>
#include <iostream>
#include <queue>

namespace net {

	Connection::Connection(asio::ip::tcp::socket socket) : socket_(std::move(socket)), receivedSize_(0) {
	}

	Connection::~Connection() {
		socket_.close();
	}

	void Connection::send(const google::protobuf::MessageLite& message) {
		mutex_.lock();
		//sendBuffer_.emplace_back(message);
		mutex_.unlock();
		auto& packet = sendBuffer_.back();
		asio::async_write(socket_, asio::buffer(packet.getData(), packet.getSize()),
			asio::transfer_exactly(packet.getSize()),
			[this](std::error_code ec, std::size_t length) {
			
			mutex_.lock();
			//sendBuffer_.pop_front();
			mutex_.unlock();
			
			if (ec) {
				std::cout << "Connection::send:" << ec.message() << std::endl;
				disconnect();
			}
		});
	}

	void Connection::sendHeader(const google::protobuf::MessageLite& message) {
		ProtobufMessage& protobufMessage = sendBuffer_.front();

		//ProtobufMessage& protobufMessage = sendBuffer_.push(message);

		asio::async_write(socket_, asio::buffer(protobufMessage.getData(), protobufMessage.getSize()),
			asio::transfer_exactly(protobufMessage.getSize()),
			[this, pb = std::move(protobufMessage)](std::error_code ec, std::size_t length) {
			
			//sendBuffer_.pop_front();
			
			if (ec) {
				std::cout << "Connection::send:" << ec.message() << std::endl;
				disconnect();
			}
		});
		sendBuffer_.pop();
	}

	void Connection::sendBody(net::ProtobufMessage&& protobufMessage) {

	}

	void Connection::readHeader() {
		receiveBuffer_.clear();
		
		asio::async_read(socket_,
			asio::buffer(&receivedSize_, sizeof(receivedSize_)),			
			asio::transfer_exactly(receiveBuffer_.getSize()),
			[this](std::error_code ec, std::size_t size) {

			receivedSize_ = asio::detail::socket_ops::network_to_host_short(receivedSize_);
			receivedSize_ = asio::detail::socket_ops::host_to_network_short(receivedSize_);
			static_assert(sizeof(asio::detail::u_short_type) == sizeof(receivedSize_));

			if (!ec) {
				readBody();
			} else {
				std::cout << "Connection::readHeader:" << ec.message() << std::endl;
				disconnect();
			}
		});
	}

	void Connection::readBody() {
		receiveBuffer_.reserveBodySize();
		asio::async_read(socket_,
			asio::buffer(receiveBuffer_.getBodyData(), receiveBuffer_.getBodySize()),
			asio::transfer_exactly(receiveBuffer_.getBodySize()),
			[this](std::error_code ec, std::size_t length) {
			if (!ec) {
				if (receiveHandler_) {
					receiveHandler_(receiveBuffer_);
				}
				readHeader();
			} else {
				std::cout << "Connection::readBody:" << ec.message() << std::endl;
				disconnect();
			}
		});
	}

	void Connection::disconnect() {
		socket_.close();
		if (disconnectHandler_) {
			disconnectHandler_();
		}
	}

} // Namespace net.
