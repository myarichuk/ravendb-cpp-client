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

#find_package(GTest MODULE REQUIRED)
target_link_libraries(Raven_CppClient_Tests ZLIB::ZLIB Raven_CppClient stdc++fs gtest gtest_main)

add_test(AllTestsInMain Raven_CppClient_Tests)

target_link_libraries(Raven_CppClient_Tests curl ssl crypto pthread)

