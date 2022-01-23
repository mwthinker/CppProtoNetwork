cmake_minimum_required(VERSION 3.21)
project(CppProtoNetwork_Example
	DESCRIPTION
		"A example of using the CppProtoNetwork_Example library"
	LANGUAGES
		CXX
)

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

if (MSVC)
	target_compile_options(CppProtoNetwork_Example
		PRIVATE
			"/permissive-"
	)
endif ()

set_target_properties(CppProtoNetwork_Example
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)
