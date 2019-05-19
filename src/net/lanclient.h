#ifndef CPPPROTONETWORK_NET_LANCLIENT_H
#define CPPPROTONETWORK_NET_LANCLIENT_H

#include "protobufmessage.h"
#include "connection.h"

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#include <atomic>
#include <thread>
#include <iostream>

namespace net {

	using ConnectHandler = std::function<void(std::error_code ec)>;

	template <class Message>
	using LanReceiveHandler = std::function<void(const Message& message, std::error_code ec)>;

	class LanServer {
	public:
		~LanServer() {
		}

		LanServer(asio::io_service& ioService, int port) : socket_(ioService),
			remoteEndpoint_(asio::ip::address_v4::broadcast(), port) {
			//remoteEndpoint_(asio::ip::address::from_string("127.0.0.1"), port) {
			// asio::ip::address::from_string("192.168.1.4")
			std::error_code ec;

			socket_.open(remoteEndpoint_.protocol(), ec);
			if (ec)
				std::cout << ec.message() << "1\n";

			std::cout << remoteEndpoint_.address() << ": " << remoteEndpoint_.port() << " : " << "        11\n";
			socket_.set_option(asio::socket_base::reuse_address(true), ec);
			if (ec)
				std::cout << ec.message() << "2\n";
			socket_.set_option(asio::socket_base::broadcast(true), ec);
			if (ec)
				std::cout << ec.message() << "3\n";

			//socket_.bind(remoteEndpoint_, ec);
			if (ec)
				std::cout << ec.message() << "4\n";
		}

		void send(const google::protobuf::MessageLite& message) {
			ProtobufMessage protobufMessage;
			sendBuffer_.acquire(protobufMessage);
			protobufMessage.setBuffer(message);

			const char* data = protobufMessage.getData();
			size_t size = protobufMessage.getSize();

			socket_.async_send_to(asio::buffer(data, size), remoteEndpoint_,
				[this, pb = std::move(protobufMessage)](std::error_code ec, std::size_t length) mutable {

				if (pb.getBodySize() > 1024) {
					//disconnect(make_error_code(Error::MESSAGE_MAX_SIZE));
					std::cout << pb.getBodySize() << std::endl;
				} else if (ec) {
					//disconnect(ec);
					std::cout << ec.message() << std::endl;
				}

				sendBuffer_.release(std::move(pb));
			});
		}

	private:
		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		ProtobufMessageQueue sendBuffer_;
	};

	class LanClient {
	public:
		~LanClient() {
		}

		LanClient(asio::io_service& ioService, int port)
			: socket_(ioService),
			remoteEndpoint_(asio::ip::address_v4::broadcast(), port) {
			//remoteEndpoint_(asio::ip::address::from_string("127.0.0.1"), port) {
		
			std::error_code ec;

			socket_.open(remoteEndpoint_.protocol(), ec);
			if (ec)
				std::cout << ec.message() << "1\n";

			std::cout << remoteEndpoint_.address() << ": " << remoteEndpoint_.port() << " : " << "        11\n";
			socket_.set_option(asio::socket_base::reuse_address(true), ec);
			if (ec)
				std::cout << ec.message() << "2\n";
			socket_.set_option(asio::socket_base::broadcast(true), ec);
			if (ec)
				std::cout << ec.message() << "3\n";

			//socket_.bind(remoteEndpoint_, ec);
			//if (ec)
				//std::cout << ec.message() << "4\n";
		}

		void connect(const std::string& ip, int port) {
		}
		
		template <class Message>
		void setReceiveHandler(int port, LanReceiveHandler<Message>&& receiveHandler) {
			static_assert(std::is_base_of<google::protobuf::MessageLite, Message>::value,
				"template type must have google::protobuf::MessageLite as base class");

			Message protocolMessage;
			receiveHandler_ = [protocolMessage, messageHandler = std::forward<LanReceiveHandler<Message>>(receiveHandler)]
			(char* data, int size, std::error_code ec) mutable {
				protocolMessage.Clear();
				if (ec) {
					messageHandler(protocolMessage, ec);
				} else {
					bool valid = protocolMessage.ParseFromArray(data, size);
					if (valid) {
						messageHandler(protocolMessage, ec);
					} else {
						messageHandler(protocolMessage, make_error_code(Error::PROTOBUF_PROTOCOL_ERROR));
					}
				}
			};
			
			asyncReceive();
		}

	private:
		using InternalReceiveHandler = std::function<void(char* data, int size, std::error_code ec)>;

		void asyncReceive() {
			socket_.async_receive_from(
				asio::buffer(recvBuffer_), remoteEndpoint_,
				[&](std::error_code ec, std::size_t bytesTransferred) {

					const int headerSize = 2;
					int protoSize = recvBuffer_[0] * 256 + recvBuffer_[1];
					std::cout << "Message: " << bytesTransferred << std::endl;
					if (bytesTransferred != headerSize + protoSize) {
						recvBuffer_[0] = 0;
						recvBuffer_[1] = 0;
						receiveHandler_(recvBuffer_.data(), 0, make_error_code(Error::MESSAGE_MAX_SIZE));
						return;
					}
					receiveHandler_(recvBuffer_.data() + headerSize, protoSize, make_error_code(Error::NONE));
				});
		}
		
		asio::ip::udp::endpoint remoteEndpoint_;
		asio::ip::udp::socket socket_;
		InternalReceiveHandler receiveHandler_;
		
		std::array<char, 1024> recvBuffer_;
	};

} // Namespace net.

#endif // CPPPROTONETWORK_NET_LANCLIENT_H
