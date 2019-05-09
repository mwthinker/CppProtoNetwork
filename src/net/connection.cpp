#include "connection.h"

#include <google/protobuf/arena.h>
#include <asio.hpp>

#include <iostream>
#include <queue>

namespace net {

	namespace {
		
		constexpr int MAX_SIZE = 1024;

		const ConnectionErrorCategory connectionErrorCategory{};		
		
	}

	const char* ConnectionErrorCategory::name() const noexcept
	{
		return "connection";
	}

	std::string ConnectionErrorCategory::message(int ev) const
	{		
		switch (static_cast<Error>(ev)) {
			case Error::MESSAGE_MAX_SIZE:
				return "received message to big";
			case Error::PROTOBUF_PROTOCOL_ERROR:
				return "protobuf protocol error";
			default:
				return "(unrecognized error)";
		}
	}

	std::error_code make_error_code(Error e)
	{
		return {static_cast<int>(e), connectionErrorCategory};
	}

	Connection::Connection(asio::ip::tcp::socket socket) : socket_(std::move(socket)) {
	}

	Connection::~Connection() {
		disconnect(make_error_code(Error::NONE));
	}

	void Connection::setReceiveHandler(const ReceiveHandler& messageHandler) {		
		receiveHandler_ = messageHandler;
		/*
		receiveHandler_ = [messageHandler, this]
		(const net::ProtobufMessage& message, std::error_code ec) mutable {
			MessageLitePtr messageLite;
			//receiveBuffer_.acquire(messageLite);
			messageLite->Clear();

			bool valid = messageLite->ParseFromArray(message.getBodyData(), message.getBodySize());
			if (valid) {
				messageHandler(std::move(messageLite), make_error_code(Error::NONE));
			} else {
				messageHandler(std::move(messageLite), make_error_code(Error::PROTOBUF_PROTOCOL_ERROR));
			}
		};
		*/
	}

	void Connection::setDisconnectHandler(const DisconnectHandler& disconnectHandler) {
		disconnectHandler_ = disconnectHandler;
	}

	void Connection::send(const google::protobuf::MessageLite& message) {
		ProtobufMessage protobufMessage;
		sendBuffer_.acquire(protobufMessage);
		protobufMessage.setBuffer(message);

		const char* data = protobufMessage.getData();
		size_t size = protobufMessage.getSize();
		asio::async_write(socket_, asio::buffer(data, size), asio::transfer_exactly(size),
			[this, pb = std::move(protobufMessage)] (std::error_code ec, std::size_t length) mutable {
			
			if (pb.getBodySize() > MAX_SIZE) {
				disconnect(make_error_code(Error::MESSAGE_MAX_SIZE));
			} else if (ec) {
				disconnect(ec);
			}

			sendBuffer_.release(std::move(pb));
		});
	}

	void Connection::readHeader() {
		receiveBuffer_.acquire(receiveMessage_);

		asio::async_read(socket_,
			asio::buffer(receiveMessage_.getData(), receiveMessage_.getHeaderSize()),
			asio::transfer_exactly(receiveMessage_.getHeaderSize()),
			[this] (std::error_code ec, std::size_t size) {

			if (receiveMessage_.getBodySize() > MAX_SIZE) {
				disconnect(make_error_code(Error::MESSAGE_MAX_SIZE));
			} else if (ec) {
				disconnect(ec);
			} else {
				readBody();
			}
		});
	}

	void Connection::readBody() {
		receiveMessage_.reserveBodySize();
		asio::async_read(socket_,
			asio::buffer(receiveMessage_.getBodyData(), receiveMessage_.getBodySize()),
			asio::transfer_exactly(receiveMessage_.getBodySize()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					disconnect(ec);
				} else {
					receiveHandler_(std::move(receiveMessage_), make_error_code(Error::NONE));
					readHeader();
				}
			});
	}

	void Connection::disconnect(std::error_code ec) {
		socket_.close();

		if (disconnectHandler_) {
			disconnectHandler_(ec);
		}
	}

	void Connection::release(ProtobufMessage&& message) {
		receiveBuffer_.release(std::move(message));
	}

} // Namespace net.
