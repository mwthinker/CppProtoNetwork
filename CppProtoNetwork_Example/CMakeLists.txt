cmake_minimum_required(VERSION 3.1...3.14)
project(CppProtoNetwork_Example
	DESCRIPTION
		"A example of using the CppProtoNetwork_Example library"
	LANGUAGES
		CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generated code for protobuf from *.proto files
add_subdirectory(CppProtoNetwork_ExampleProtocol)

add_executable(CppProtoNetwork_Example
	src/main.cpp
)

target_link_libraries(CppProtoNetwork_Example
	PRIVATE
		CppProtoNetwork
		CppProtoNetwork_ExampleProtocol
)
