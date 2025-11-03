cmake_minimum_required (VERSION 3.20)


project ("libjson" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_library(libjson INTERFACE)

target_include_directories(
	libjson 
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET libjson PROPERTY CXX_STANDARD 23)
endif()

option(BUILD_TESTS "Build the unit tests" OFF)


if(BUILD_TESTS)
	enable_testing()
	
	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
	)
	FetchContent_MakeAvailable(googletest)

	file(GLOB_RECURSE libjson_test_sources tests/*.cpp)
	add_executable(libjson_tests ${libjson_test_sources})
	target_link_libraries(
		libjson_tests
		PRIVATE
		libjson
		GTest::gtest_main
	)

	include(GoogleTest)
    gtest_discover_tests(libjson_tests)


endif()