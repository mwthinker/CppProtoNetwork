#include <net/server.h>
#include <net/client.h>

#include <message.pb.h>

#include <iostream>
#include <functional>
#include <mutex>
#include <atomic>
#include <string>

using namespace std::literals::chrono_literals;
using namespace net;
using namespace std::chrono_literals;

void runServer() {
	std::cout << "Start server\n";
	auto server = Server::create();	

	server->setConnectHandler([&](const RemoteClientPtr& remoteClientPtr) {
		std::cout << "New Connection\n";	

		remoteClientPtr->setReceiveHandler<message::Wrapper>([](const message::Wrapper& wrapper, std::error_code ec) {
			std::cout << wrapper.text() << "\n";
		});

		remoteClientPtr->setDisconnectHandler([](std::error_code ec) {
			std::cout << "Disconnected\n";
		});
	});

	try {
		server->connect(5012);
	} catch (asio::system_error se) {
		std::cout << se.what() << "\n";
		return;
	}

	std::cout << "Not allowing more connections: [y]\n";
	std::cout << "Allowing more connections: [n]\n";
	std::cout << "Exit: [x]\n";

	message::Wrapper wrapper;

	std::string input;
	do {
		std::cout << "Text: ";
		std::getline(std::cin, input);

		wrapper.Clear();
		wrapper.set_text(input);

		if (input == "y") {
			std::cout << "Not allowing more connections\n";
			try {
				server->setAllowingNewConnections(false);
			}
			catch (asio::system_error e) {
				std::cout << "x: "<< e.what() << "\n";
			}
		}
		if (input == "n") {
			std::cout << "Allowing more connections\n";
			try {
				server->setAllowingNewConnections(true);
			}
			catch (asio::system_error e) {
				std::cout << "x: " << e.what() << "\n";
			}
		}
		if (input == "x") {
			break;
		}
		server->sendToAll(wrapper);
	} while (!input.empty());
}

void runClient() {
	std::cout << "Start client\n";
	std::cout << "Exit: [x]\n";

	auto client = Client::create();

	std::atomic<bool> connected = false;
	std::mutex mutex;
	std::condition_variable cv;
	
	client->setReceiveHandler<message::Wrapper>([client](const message::Wrapper& message, std::error_code ec) {
		std::cout << message.text() << "\n";
	});

	client->setDisconnectHandler([&](std::error_code ec) {
		connected = false;
		std::cout << "Disconnected: " << ec.message() << "\n";
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
			if (input == "x") {
				break;
			}
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
