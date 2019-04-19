cmake_minimum_required(VERSION 3.1...3.14)
project(CppProtocolTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(protobuf REQUIRED)

set(PROTO_FILES
	"message.proto"
)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${PROTO_FILES})

add_library(CppProtocolTest ${PROTO_HEADER} ${PROTO_SRC} ${PROTO_FILES})

target_include_directories(CppProtocolTest
	PUBLIC
		${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(CppProtocolTest
	PUBLIC
		protobuf::libprotobuf
	PRIVATE
		protobuf::libprotoc
)
