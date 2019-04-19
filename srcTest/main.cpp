#include <net/server.h>
#include <net/client.h>

#include <message.pb.h>
#include <asio.hpp>

#include <memory>
#include <iostream>
#include <functional>
#include <mutex>
#include <condition_variable>

using namespace std::literals::chrono_literals;
using namespace net;

void runServer() {
	std::cout << "Start server\n";
	auto server = Server::create();
	server->connect(5012);

	server->setConnectHandler([&](const RemoteClientPtr& remoteClientPtr) {
		std::cout << "New Connection\n";	

		remoteClientPtr->setReceiveHandler<message::Wrapper>([](const message::Wrapper& wrapper) {
			std::cout << wrapper.text() << "\n";
		});

		remoteClientPtr->setDisconnectHandler([]() {
			std::cout << "Disconnected\n";
		});
	});

	std::string input;
	do {
		std::cout << "Text: ";
		std::getline(std::cin, input);

		message::Wrapper wrapper;
		wrapper.set_text(input);
		
		message::Wrapper w = std::move(wrapper);

		if (input == "x") {
			std::cout << "Not allowing more connections\n";
			try {
				server->setAllowingNewConnections(false);
			}
			catch (asio::system_error e) {
				std::cout << "x: "<< e.what() << "\n";
			}
		}
		if (input == "z") {
			std::cout << "Allowing more connections\n";
			try {
				server->setAllowingNewConnections(true);
			}
			catch (asio::system_error e) {
				std::cout << "x: " << e.what() << "\n";
			}
		}

		server->sendToAll(wrapper);
	} while (!input.empty());
}

void runClient() {
	std::cout << "Start client\n";
	auto client = Client::create();

	bool connected = false;
	std::mutex mutex;
	std::condition_variable cv;
	
	client->setReceiveHandler<message::Wrapper>([](const message::Wrapper& wrapper) {
		std::cout << wrapper.text() << "\n";
	});

	client->setDisconnectHandler([&]() {
		std::lock_guard<std::mutex> lock(mutex);
		connected = false;
		std::cout << "Disconnected\n";
	});
	
	client->setConnectHandler([&](std::error_code ec) {
		std::lock_guard<std::mutex> lock(mutex);
		if (ec) {
			std::cout << ec.message() << "\n";
			connected = false;
		} else {
			connected = true;
		}
		cv.notify_all();
	});

	std::unique_lock<std::mutex> lock(mutex);
	client->connect("127.0.0.1", 5012);
	cv.wait(lock);
	
	if (connected) {
		std::cout << "Connected\n";
		std::string input;
		do {
			std::cout << "Text: ";
			std::getline(std::cin, input);

			message::Wrapper wrapper;
			wrapper.set_text(input);			

			client->send(wrapper);
		} while (!input.empty() && connected);
	}
}

void testNetwork(int argc, const char* argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "-s") == 0) {
			runServer();
		} else if (strcmp(argv[1], "-c") == 0) {
			runClient();
		}
	} else {
		runServer();
	}
}

int main(int argc, const char* argv[]) {
	testNetwork(argc, argv);
	return 0;
}
