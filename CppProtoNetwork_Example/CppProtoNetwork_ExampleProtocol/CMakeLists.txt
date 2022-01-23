cmake_minimum_required(VERSION 3.21)
project(CppProtoNetwork_ExampleProtocol)

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

set_target_properties(CppProtoNetwork_ExampleProtocol
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)
