cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient_Tests)

include_directories(
	${GTEST_INCLUDE_DIRS}
)

FILE(GLOB RAVEN_TEST_HEADERS ./*.h)
FILE(GLOB RAVEN_TEST_CPPS ./*.cpp)

if(MSVC)
	add_compile_options(/bigobj)
endif()

option(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." ON)
option(CMAKE_FIND_DEBUG_MODE ON)

add_executable(Raven_CppClient_Tests ${RAVEN_TEST_HEADERS} ${RAVEN_TEST_CPPS})

target_include_directories(Raven_CppClient_Tests PRIVATE . .. ../Raven.CppClient ../external ${CMAKE_INCLUDE_PATH})

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(threads REQUIRED)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(Raven_CppClient_Tests GTest::gtest Raven_CppClient_static OpenSSL::SSL OpenSSL::Crypto ${CURL_LIBRARIES} Threads::Threads)

if(NOT WIN32)
	target_link_libraries(Raven_CppClient_Tests PRIVATE stdc++fs)
endif()

include(GoogleTest)
gtest_discover_tests(Raven_CppClient_Tests)  

