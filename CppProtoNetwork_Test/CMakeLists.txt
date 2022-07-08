cmake_minimum_required(VERSION 3.23)
project(CppProtoNetwork_Test
	DESCRIPTION
		"Test the project CppProtoNetwork_Test using google test"
	LANGUAGES
		CXX
)

find_package(GTest CONFIG)
if (GTest_FOUND)
	enable_testing()

	add_executable(CppProtoNetwork_Test
		src/main.cpp
	)

	target_link_libraries(CppProtoNetwork_Test
		PRIVATE
			CppProtoNetwork
			CppProtoNetwork_ExampleProtocol
			fmt::fmt-header-only
			GTest::gtest GTest::gtest_main # GTest::gmock_main GTest::gmock
	)

	if (MSVC)
		target_compile_options(CppProtoNetwork_Test
			PRIVATE
				"/permissive-"
				"/wd4251" # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
		)
	endif (MSVC)

	set_target_properties(CppProtoNetwork_Test
		PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED YES
			CXX_EXTENSIONS NO
	)
	
	include(GoogleTest)
	gtest_discover_tests(CppProtoNetwork_Test)

else (GTest_FOUND)
	message(STATUS "GTest not found, CppProtoNetwork_Test not created")
endif (GTest_FOUND)
