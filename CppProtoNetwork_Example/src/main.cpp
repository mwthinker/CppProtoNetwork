#include <net/server.h>
#include <net/client.h>
#include <net/lanudpsender.h>
#include <net/lanudpreceiver.h>

#include <message.pb.h>

#include <functional>
#include <string>
#include <fmt/format.h>

using namespace std::chrono_literals;

constexpr int Port = 5013;
const std::string LocalHost = "127.0.0.1";

constexpr int LanPort = 32012;

template <std::invocable T>
void repeatTimer(asio::steady_timer& timer, std::chrono::seconds interval, T callback) {
	timer.expires_after(interval);
	timer.async_wait([&timer, interval, callback](const std::error_code& error) {
		if (error) {
			fmt::print("repeatTimer {}\n", error.message());
			return;
		}
		callback();
		::repeatTimer<T>(timer, interval, callback);
	});
}

void runServer() {
	std::cout << "Start server\n";
	asio::io_context ioContext;

	auto server = net::Server::create(ioContext);

	server->setConnectHandler([&](const net::RemoteClientPtr& remoteClientPtr) {
		fmt::print("New Connection\n");

		remoteClientPtr->setReceiveHandler<message::Wrapper>([](const message::Wrapper& wrapper, std::error_code ec) {
			fmt::print("Received: {}\n", wrapper.text());
		});

		remoteClientPtr->setDisconnectHandler([](std::error_code ec) {
			fmt::print("Disconnected\n");
		});
	});

	try {
		server->connect(Port);
	} catch (asio::system_error se) {
		fmt::print("{}\n", se.what());
		return;
	}
	
	asio::steady_timer timer{ioContext};
	int timerNbr = 0;
	repeatTimer(timer, 2s, [&timerNbr, &server]() {
		auto text = fmt::format("Server DATA {}\n", ++timerNbr);
		fmt::print("Send {}\n", text);
		message::Wrapper wrapper;
		wrapper.set_text(text);
		server->sendToAll(wrapper);
	});

	ioContext.run();
}

void runClient() {
	fmt::print("Start client\n");

	asio::io_context ioContext;
	auto client = net::Client::create(ioContext);
	bool connected = false;
	client->setReceiveHandler<message::Wrapper>([](const message::Wrapper& message, std::error_code ec) {
		fmt::print("Received: {}\n", message.text());
	});
	client->setDisconnectHandler([&](std::error_code ec) {
		connected = false;
		fmt::print("Disconnected: {}\n", ec.message());
	});
	client->setConnectHandler([&](std::error_code ec) {
		if (ec) {
			fmt::print("{}\n", ec.message());
			connected = false;
		} else {
			fmt::print("Connected\n");
			connected = true;
		}
	});

	client->connect(LocalHost, Port);
	
	asio::steady_timer timer{ioContext};
	int timerNbr = 0;
	repeatTimer(timer, 3s, [&timerNbr, &client]() {
		auto text = fmt::format("Client DATA {}\n", ++timerNbr);
		message::Wrapper wrapper;
		wrapper.set_text(text);
		client->send(wrapper);
	});

	ioContext.run();
}

void runServerLan() {
	fmt::print("Start server LAN\n");

	asio::io_context ioContext;
	net::LanUdpSender lanUdpSender{ioContext};

	bool disconnected = false;

	lanUdpSender.setDisconnectHandler([&](std::error_code ec) {
		disconnected = true;
		fmt::print("Disconnected: {}\n", ec.message());
	});

	message::Wrapper wrapper;
	wrapper.set_text("hello");
	lanUdpSender.setMessage(wrapper);
	
	lanUdpSender.connect(LanPort);

	while (!disconnected) {
		ioContext.run_one();
	}
}

void runClientLan() {
	fmt::print("Start client LAN\n");

	asio::io_context ioContext;
	net::LanUdpReceiver lanUdpReceiver{ioContext};

	lanUdpReceiver.setReceiveHandler<message::Wrapper>([](const net::Meta& meta, const message::Wrapper& wrapper, std::error_code ec) {
		fmt::print("{} | {}\n", meta.endpoint.address().to_string(), meta.endpoint.port());
		fmt::print("Message: {}\n", wrapper.text());
	});

	lanUdpReceiver.connect(LanPort);
	ioContext.run();
}

void testNetwork(int argc, const char* argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "-h") == 0|| strcmp(argv[1], "--help") == 0) {
			fmt::print("Test\n");
		} else if (strcmp(argv[1], "-s") == 0) {
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
