# net::CppProtoNetwork [![CI build](https://github.com/mwthinker/CppProtoNetwork/actions/workflows/ci.yml/badge.svg)](https://github.com/mwthinker/CppProtoNetwork/actions/workflows/ci.yml)
## About
A network library using TCP sockets. Handles server and clients. Uses standalone [Asio](https://think-async.com/Asio/) (no boost) library. Data is serialized using [Protobuf](https://developers.google.com/protocol-buffers/).

It uses C++20 and the C++ standard library.

## Requirements
* [CMake](https://cmake.org/)
* C++20 compliant compiler
* [vcpkg](https://github.com/microsoft/vcpkg)

### vcpkg
Either define CMAKE_TOOLCHAIN_FILE in cmake to use the one provided by vcpkg or define a enviromental variable VCPKG_ROOT to the vcpkg install directory.

Install following packages:
```
# Inside project dir (assuming in a unix host)
mkdir build
cmake --preset=unix ..
```

## Example code
### Proto file

```c
syntax = "proto3";

package message;

message Wrapper {
    string text = 1;
}
```

### Server code

```C++
#include <net/server.h>
#include <message.pb.h> // Generated code by protobuf.

// Some code ...

auto server = Server::create();
// Must setup connections handlers before server is connected.
// Data is called from the server's internal thread, data Therefore need to be
// protected.
server->setConnectHandler([](const RemoteClientPtr& remoteClientPtr) {
    std::cout << "New Connection\n";	

    remoteClientPtr->setReceiveHandler<message::Wrapper>([](const message::Wrapper& wrapper, std::error_code ec) {
        std::cout << wrapper.text() << "\n";
    });

    remoteClientPtr->setDisconnectHandler([](std::error_code ec) {
        std::cout << "Disconnected\n";
    });
});

// Some code ...

// Start listening to incomming connections on port 5012
try {
    server->connect(5012);
} catch (asio::system_error se) {
    std::cout << se.what() << "\n";
    return;
}

// Some code ...

// Set some data
message::Wrapper wrapper;
wrapper.set_text("Hellor world");

// Send to all conencted clients.
server->sendToAll(wrapper);

// Some code ...

void sendToSpecificClient(RemoteClientPtr remoteClientPtr) {
    message::Wrapper wrapper;
    wrapper.set_text("Hello remote client!");

    remoteClientPtr->send(wrapper);
}

```

### Client code

```C++
#include <net/client.h>
#include <message.pb.h> // Generated code by protobuf.

// Some code ...

auto client = Client::create();
// Must setup connections handlers before client is connected.
// Data is called from the clients's internal thread, data Therefore need to be
// protected.
client->setReceiveHandler<message::Wrapper>([](const message::Wrapper& message, std::error_code ec) {
    std::cout << message.text() << "\n";
});

client->setConnectHandler([](std::error_code ec) {
    if (ec) {
        std::cout << ec.message() << "\n";
    } else {
        std::cout << "Jippi! Is connected to server!" << "\n";
    }
});

client->setDisconnectHandler([](std::error_code ec) {	
    std::cout << "Disconnected: " << ec.message() << "\n";
});

// Some code ...

// Connect to server.
client->connect("127.0.0.1", 5012);

// Some code ...

// Set some data
message::Wrapper wrapper;
wrapper.set_text("Hellor world");

// Send to server
client->send(wrapper);

```

## Open source
The project is under the MIT license (see LICENSE.md).
