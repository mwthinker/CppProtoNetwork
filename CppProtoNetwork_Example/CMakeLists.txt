cmake_minimum_required(VERSION 3.23)
project(CppProtoNetwork_Example
	DESCRIPTION
		"A example of using the CppProtoNetwork_Example library"
	LANGUAGES
		CXX
)

# Generated code for protobuf from *.proto files
add_subdirectory(CppProtoNetwork_ExampleProtocol)

find_package(fmt CONFIG REQUIRED)

add_executable(CppProtoNetwork_Example
	src/main.cpp
)

target_link_libraries(CppProtoNetwork_Example
	PRIVATE
		CppProtoNetwork
		CppProtoNetwork_ExampleProtocol
		fmt::fmt-header-only
)

if (MSVC)
	target_compile_options(CppProtoNetwork_Example
		PRIVATE
			"/permissive-"
			"/wd4251" # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
	)
endif ()

set_target_properties(CppProtoNetwork_Example
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)
