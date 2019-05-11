cmake_minimum_required(VERSION 3.1...3.14)

set(VCPKG_ROOT $ENV{VCPKG_ROOT})
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
		CACHE STRING ""
	)
endif()

project(CppProtoNetwork
	DESCRIPTION
		"C++ Server/Client library using ASIO and protobuf"
	LANGUAGES
		CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
	src/net/bufferqueue.cpp
	src/net/bufferqueue.h
	src/net/client.cpp
	src/net/client.h
	src/net/connection.cpp
	src/net/connection.h
	src/net/protobufmessage.cpp
	src/net/protobufmessage.h
	src/net/remoteclient.cpp
	src/net/remoteclient.h
	src/net/server.cpp
	src/net/server.h
)

add_library(CppProtoNetwork ${SOURCES})

find_package(Threads REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# ASIO non-boost version.
target_compile_definitions(CppProtoNetwork
	PUBLIC
		ASIO_STANDALONE
)

if (MSVC)
	# ASIO related.
	target_compile_definitions(CppProtoNetwork
		PUBLIC
			_WIN32_WINNT=_WIN32_WINNT_WS03
			_WINSOCK_DEPRECATED_NO_WARNINGS
	)
endif (MSVC)

target_include_directories(CppProtoNetwork
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

target_link_libraries(CppProtoNetwork
	PUBLIC
		Threads::Threads
		protobuf::libprotobuf
)

message(STATUS "CppProtoNetworkTest is available to add: -DCppProtoNetworkTest=1")
option(CppProtoNetworkTest "Add CppProtoNetworkTest project." OFF)

if (CppProtoNetworkTest)
	# Generated code for protobuf from *.proto files
	add_subdirectory(CppProtocolTest)

	set(SOURCES_TEST
		srcTest/main.cpp
	)

	add_executable(CppProtoNetworkTest ${SOURCES_TEST})

	target_include_directories(CppProtoNetworkTest
		PUBLIC
			${CMAKE_CURRENT_SOURCE_DIR}/srcTest
	)

	target_link_libraries(CppProtoNetworkTest
		PRIVATE
			CppProtocolTest
			CppProtoNetwork
	)
	
endif (CppProtoNetworkTest)
