cmake_minimum_required(VERSION 3.24)
project(CppProtoNetwork
	DESCRIPTION
		"C++ Server/Client library using ASIO and protobuf"
	LANGUAGES
		CXX
)

set(SOURCES
	src/net/detail/connection.cpp
	src/net/detail/connection.h
	
	src/net/auxiliary.cpp
	src/net/auxiliary.h
	src/net/client.cpp
	src/net/client.h
	src/net/iocontext.cpp
	src/net/iocontext.h
	src/net/lanudpreceiver.cpp
	src/net/lanudpreceiver.h
	src/net/lanudpsender.cpp
	src/net/lanudpsender.h
	src/net/protobufmessage.cpp
	src/net/protobufmessage.h
	src/net/protobufmessagequeue.cpp
	src/net/protobufmessagequeue.h
	src/net/remoteclient.cpp
	src/net/remoteclient.h
	src/net/server.cpp
	src/net/server.h
	src/net/timer.cpp
	src/net/timer.h

	vcpkg.json
)

add_library(CppProtoNetwork ${SOURCES})

set_property(GLOBAL PROPERTY USE_FOLDERS On)
source_group(TREE
	${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${SOURCES}
)

find_package(asio CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# ASIO non-boost version.
target_compile_definitions(CppProtoNetwork
	PUBLIC
		ASIO_STANDALONE
		ASIO_NO_DEPRECATED
)

if (MSVC)
	# ASIO related.
	target_compile_definitions(CppProtoNetwork
		PUBLIC
			_WIN32_WINNT=_WIN32_WINNT_WS03
			_WINSOCK_DEPRECATED_NO_WARNINGS
	)
endif ()

target_include_directories(CppProtoNetwork
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

target_link_libraries(CppProtoNetwork
	PUBLIC
		Threads::Threads
		protobuf::libprotobuf
		asio asio::asio
)

if (MSVC)
	target_compile_options(CppProtoNetwork
		PRIVATE
			"/permissive-"
			"/wd4251" # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
	)
endif ()

set_target_properties(CppProtoNetwork
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

message(STATUS "CppProtoNetwork_Example is available to add: -DCppProtoNetwork_Example=1")
option(CppProtoNetwork_Example "Add CppProtoNetwork_Example project." OFF)
if (CppProtoNetwork_Example)
	add_subdirectory(CppProtoNetwork_Example)
endif ()

message(STATUS "CppProtoNetwork_Example is available to add: -DCppProtoNetwork_Test=1")
option(CppProtoNetwork_Test "Add CppProtoNetwork_Test project." OFF)
if (CppProtoNetwork_Test)
	add_subdirectory(CppProtoNetwork_Test)
endif ()
