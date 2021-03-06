cmake_minimum_required(VERSION 3.1...3.14)
project(CppProtoNetwork_ExampleProtocol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)

set(PROTO_FILES
	"message.proto"
)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${PROTO_FILES})

add_library(CppProtoNetwork_ExampleProtocol ${PROTO_HEADER} ${PROTO_SRC} ${PROTO_FILES})
if (MSVC)
	target_compile_options(CppProtoNetwork_ExampleProtocol
		PUBLIC
			"/wd4005" # 'identifier' : macro redefinition
			"/wd4251" # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
			"/wd4244" # An integer type is converted to a smaller integer type
	)
endif ()

target_include_directories(CppProtoNetwork_ExampleProtocol
	PUBLIC
		${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(CppProtoNetwork_ExampleProtocol
	PUBLIC
		protobuf::libprotobuf
	PRIVATE
		protobuf::libprotoc
)
