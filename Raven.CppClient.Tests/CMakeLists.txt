cmake_minimum_required(VERSION 3.13)

FILE(GLOB RAVEN_TEST_HEADERS *.h)
FILE(GLOB RAVEN_TEST_CPPS *.cpp)

if(MSVC)
	add_compile_options(/bigobj)
endif()


add_executable(Raven_CppClient_Tests ${RAVEN_TEST_HEADERS} ${RAVEN_TEST_CPPS})

#login files
if(WIN32)
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../win_re_config/secured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../win_re_config/unsecured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
else()
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../linux_re_config/secured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../linux_re_config/unsecured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
endif()

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(threads REQUIRED)

find_package(GTest CONFIG REQUIRED)

target_include_directories(Raven_CppClient_Tests PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} . .. ../Raven.CppClient ../external ${CMAKE_INCLUDE_PATH})

target_link_libraries(Raven_CppClient_Tests GTest::gtest GTest::gtest_main Raven_CppClient OpenSSL::SSL OpenSSL::Crypto ${CURL_LIBRARIES} Threads::Threads)

if(NOT WIN32)
	target_link_libraries(Raven_CppClient_Tests PRIVATE stdc++fs)
endif()

enable_testing()

include(GoogleTest)
gtest_add_tests(Raven_CppClient_Tests ${RAVEN_TEST_CPPS})  

