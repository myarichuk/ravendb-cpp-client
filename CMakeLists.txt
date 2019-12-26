cmake_minimum_required(VERSION 3.13)
include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.240.tar.gz"
    SHA1 "ca19f3769e6c80cfdd19d8b12ba5102c27b074e0"
)

project(RavenDB-CppClient)

include(CTest) 

set(CMAKE_CXX_STANDARD 17)
include(cmake/xxhash.cmake)
include_directories(${xxhash_cpp_SOURCE_DIR}/xxhash)

hunter_add_package(CURL)
#[[
find_package(CURL CONFIG REQUIRED)
target_link_libraries(... CURL::libcurl)
]]

hunter_add_package(ZLIB)
#[[
find_package(ZLIB CONFIG REQUIRED)

add_executable(boo main.c)
target_link_libraries(boo PRIVATE ZLIB::zlib)
]]

hunter_add_package(OpenSSL)
#[[
find_package(OpenSSL REQUIRED)
target_link_libraries(foo PUBLIC OpenSSL::SSL OpenSSL::Crypto)
]]

hunter_add_package(nlohmann_json)
#[[
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC nlohmann_json::nlohmann_json)
]]

hunter_add_package(GTest)
#[[
find_package(GTest CONFIG REQUIRED)

target_link_libraries(foo GTest::main) # GTest::gtest will be linked automatically
target_link_libraries(boo GTest::gtest)
]]

add_subdirectory(Raven.CppClient)

 
#choose if buildings tryouts
option(BUILD_TRYOUTS "Build the Tryouts" OFF)
if(BUILD_TRYOUTS)
    add_subdirectory(Tryouts) 
endif()

#choose if building tests
option(BUILD_TESTS "Build the Tests" ON)
if(BUILD_TESTS)
	enable_testing()
	if(WIN32)
		set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/win_re_config" CACHE INTERNAL "")
	else()
		set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/linux_re_config" CACHE INTERNAL "")
	endif()

	message("Test config folder: ${PROJECT_SOURCE_DIR}/win_re_config")

    add_subdirectory(Raven.CppClient.Tests)
endif()

#[[
#set user home directory for installation
if(WINN32)
    set(USER_HOME_DIRECTORY $ENV{HOMEPATH})
endif()
if(UNIX)
    set(USER_HOME_DIRECTORY $ENV{HOME})
endif()


if(CMAKE_INSTALL_PREFIX EQUAL "")
    set(CMAKE_INSTALL_PREFIX ${USER_HOME_DIRECTORY}/RavenDBCppClient CACHE PATH "destination directory" FORCE)
endif()

#FILE(GLOB include_files Raven.CppClient/*.h)
#FILE(GLOB external_include_files external/*)
#FILE(GLOB ca_bundle_files ca_bundle/*)

#FILE(GLOB HELLO_WORLD_APP ./hello_world.cpp)

#install(TARGETS Raven_CppClient LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
#install(FILES ${include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
#install(FILES ${external_include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/external)
#install(FILES ${ca_bundle_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/ca_bundle)
#install(FILES ${HELLO_WORLD_APP} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
]]