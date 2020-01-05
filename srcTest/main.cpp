#include <net/server.h>
#include <net/client.h>
#include <net/lanudpsender.h>
#include <net/lanudpreceiver.h>

#include <message.pb.h>

#include <iostream>
#include <functional>
#include <string>

using namespace net;
using namespace std::chrono_literals;

constexpr unsigned short PORT = 5013;
const std::string LOCALHOST = "127.0.0.1";

constexpr unsigned short LAN_PORT = 32012;

template <class T>
void repeatTimer(asio::steady_timer& timer, const std::chrono::seconds interval, T callback) {
	timer.expires_after(interval);
	timer.async_wait([&timer, interval, callback](const std::error_code& error) {
		if (error) {
			std::cout << "repeatTimer " << error.message() << "\n";
			return;
		}
		callback();
		::repeatTimer<T>(timer, interval, callback);
	});
}

void runServer() {
	std::cout << "Start server\n";
	asio::io_context ioContext;

	auto server = Server::create(ioContext);
	server->setConnectHandler([&](const RemoteClientPtr& remoteClientPtr) {
		std::cout << "New Connection\n";

		remoteClientPtr->setReceiveHandler<message::Wrapper>([](const message::Wrapper& wrapper, std::error_code ec) {
			std::cout << "Received: " << wrapper.text() << std::endl;
		});

		remoteClientPtr->setDisconnectHandler([](std::error_code ec) {
			std::cout << "Disconnected" << std::endl;
		});
	});

	try {
		server->connect(PORT);
	} catch (asio::system_error se) {
		std::cout << se.what() << "\n";
		return;
	}

	message::Wrapper wrapper;
	asio::steady_timer timer{ioContext};
	int timerNbr = 0;
	
	repeatTimer(timer, 2s, [&timerNbr, &server]() {
		std::stringstream stream;
		stream << "Server DATA " << ++timerNbr << std::endl;
		std::cout << "Send " << timerNbr <<" \n";
		message::Wrapper wrapper;
		wrapper.set_text(stream.str());
		server->sendToAll(wrapper);
	});

	ioContext.run();
}

void runClient() {
	std::cout << "Start client" << std::endl;

	asio::io_context ioContext;
	auto client = Client::create(ioContext);
	bool connected = false;
	client->setReceiveHandler<message::Wrapper>([](const message::Wrapper& message, std::error_code ec) {
		std::cout << "Received: " << message.text() << std::endl;
	});
	client->setDisconnectHandler([&](std::error_code ec) {
		connected = false;
		std::cout << "Disconnected: " << ec.message() << std::endl;
	});
	client->setConnectHandler([&](std::error_code ec) {
		if (ec) {
			std::cout << ec.message() << std::endl;
			connected = false;
		} else {
			std::cout << "Connected" << std::endl;
			connected = true;
		}
	});
	client->connect(LOCALHOST, PORT);

	int timerNbr = 0;
	asio::steady_timer timer{ioContext};
	repeatTimer(timer, 3s, [&timerNbr, &client]() {
		std::stringstream stream;
		stream << "Client DATA " << ++timerNbr;
		message::Wrapper wrapper;
		wrapper.set_text(stream.str());
		client->send(wrapper);
	});

	ioContext.run();
}

void runServerLan() {
	std::cout << "Start server lan" << std::endl;

	asio::io_context ioContext;
	LanUdpSender lanUdpSender{ioContext};

	bool disconnected = false;
	lanUdpSender.setDisconnectHandler([&](std::error_code ec) {
		disconnected = true;
		std::cout << ec.message() << std::endl;
	});

	message::Wrapper wrapper;
	wrapper.set_text("hej");
	lanUdpSender.setMessage(wrapper);
	
	lanUdpSender.connect(LAN_PORT);

	while (!disconnected) {
		ioContext.run_one();
	}
}

void runClientLan() {
	std::cout << "Start client lan" << std::endl;

	asio::io_context ioContext;
	LanUdpReceiver lanUdpReceiver{ioContext};

	lanUdpReceiver.setReceiveHandler<message::Wrapper>([](const Meta& meta, const message::Wrapper& wrapper, std::error_code ec) {
		std::cout << meta.endpoint_.address() << " | " << meta.endpoint_.port() << "\n";
		std::cout << "Message: " << wrapper.text() << std::endl;
	});

	lanUdpReceiver.connect(LAN_PORT);
	ioContext.run();
}

void testNetwork(int argc, const char* argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "-s") == 0) {
			runServer();
		} else if (strcmp(argv[1], "-c") == 0) {
			runClient();
		} else if (strcmp(argv[1], "-ss") == 0) {
			runServerLan();
		} else if (strcmp(argv[1], "-cc") == 0) {
			runClientLan();
		}
	} else {
		runServer();
	}
}

int main(int argc, const char* argv[]) {
	testNetwork(argc, argv);

	return 0;
}
