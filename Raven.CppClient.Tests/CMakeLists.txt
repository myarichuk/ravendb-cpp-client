cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient_Tests)

#add_subdirectory(lib/googletest/googletest)

FILE(GLOB RAVEN_TEST_HEADERS ./*.h)
FILE(GLOB RAVEN_TEST_CPPS ./*.cpp)

if(MSVC)
	add_compile_options(/bigobj)
endif()

add_executable(Raven_CppClient_Tests ${RAVEN_TEST_HEADERS} ${RAVEN_TEST_CPPS})

target_include_directories(Raven_CppClient_Tests PRIVATE . .. ../Raven.CppClient ../external ${CMAKE_INCLUDE_PATH})

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(threads REQUIRED)

if(MSVC)
	set(GTEST_MSVC_SEARCH MT)
endif()

find_package(GTest REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Raven_CppClient)
target_link_libraries(Raven_CppClient_Tests PRIVATE GTest::GTest GTest::Main)
target_link_libraries(Raven_CppClient_Tests PRIVATE OpenSSL::SSL OpenSSL::Crypto ${CURL_LIBRARIES} Threads::Threads)

if(NOT WIN32)
	target_link_libraries(Raven_CppClient_Tests PRIVATE stdc++fs)
endif()

add_test(AllTestsInMain Raven_CppClient_Tests)

