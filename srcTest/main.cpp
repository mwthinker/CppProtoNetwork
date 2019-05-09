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
	server->connect(5012);

	server->setConnectHandler([&](const RemoteClientPtr& remoteClientPtr) {
		std::cout << "New Connection\n";	

		remoteClientPtr->setReceiveHandler([remoteClientPtr](ProtobufMessage&& message, std::error_code ec) {
			message::Wrapper wrapper;
			bool valid = wrapper.ParseFromArray(message.getBodyData(), message.getBodySize());
			if (valid) {
				std::cout << wrapper.text() << "\n";
			} else {
				std::cout << "Protocol error\n";
			}
			remoteClientPtr->release(std::move(message));
			//auto wrapper = castProtobufMessage<message::Wrapper>(std::move(message));
			//std::cout << wrapper->text() << "\n";
			//remoteClientPtr->release(std::move(wrapper));
		});

		remoteClientPtr->setDisconnectHandler([](std::error_code ec) {
			std::cout << "Disconnected\n";
		});
	});

	std::string input;
	do {
		std::cout << "Text: ";
		std::getline(std::cin, input);

		message::Wrapper wrapper;
		google::protobuf::MessageLite& message = wrapper;

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
	auto client = Client::create();

	std::atomic<bool> connected = false;
	std::mutex mutex;
	std::condition_variable cv;
	
	client->setReceiveHandler([client](ProtobufMessage&& message, std::error_code ec) {
		message::Wrapper wrapper;
		bool valid = wrapper.ParseFromArray(message.getBodyData(), message.getBodySize());
		if (valid) {			
			std::cout << wrapper.text() << "\n";
		} else {
			std::cout << "Protocol error\n";
		}
		client->release(std::move(message));

		//auto wrapper = castProtobufMessage<message::Wrapper>(std::move(message));
		//std::cout << wrapper->text() << "\n";
		//client->release(std::move(wrapper));
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
