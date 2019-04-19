#include "packet.h"
#include "network.h"
#include "server.h"
#include "client.h"
#include "connection.h"
#include "connectionscontrol.h"

namespace net {

	bool Network::firstInstance = true;

	Network::Network(int sleepMilliseconds) : status_(NOT_ACTIVE),
		connectionsControl_(0), sleepMilliseconds_(sleepMilliseconds),
		isServer_(false), isClient_(false) {


		asio::io_service::work work(service_);
		service_.run();

	}

	Network::~Network() {
		stop();
	}

	void Network::startServer(int port) {
		if (connectionsControl_ == 0) {
            mutex_ = std::make_shared<std::mutex>();
			auto server = new Server(sleepMilliseconds_, mutex_);
			status_ = ACTIVE;
			thread_ = std::thread(&Server::run, server, port);
			connectionsControl_ = server;
			isServer_ = true;
			isClient_ = false;
		}
	}

	void Network::startClient(std::string serverIp, int port) {
		if (connectionsControl_ == 0) {
            mutex_ = std::make_shared<std::mutex>();
			auto client = new Client(sleepMilliseconds_, mutex_);
			status_ = ACTIVE;
			thread_ = std::thread(&Client::run, client, port, serverIp);
			connectionsControl_ = client;
			isServer_ = false;
			isClient_ = true;
		}
	}

	void Network::stop() {
	    // Has a active server/client?
		if (connectionsControl_ != 0) {
			connectionsControl_->close();
            thread_.join();
			delete connectionsControl_;
			connectionsControl_ = 0;
            status_ = NOT_ACTIVE;
			isServer_ = false;
			isClient_ = false;
	    }
	}

	void Network::setAcceptConnections(bool accept) {
		if (connectionsControl_ != 0) {
			connectionsControl_->setAcceptConnections(accept);
	    }
	}

	bool Network::isAcceptingConnections() const {
		if (connectionsControl_ != 0) {
			return connectionsControl_->isAcceptingConnections();
	    }
	    return true;
	}

	std::shared_ptr<Connection> Network::pollConnection() {
		if (connectionsControl_ != 0) {
			return connectionsControl_->pollConnection();
	    }
        return nullptr;
	}

	Status Network::getStatus() const {
		return status_;
	}	

} // Namespace net.
