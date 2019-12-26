cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB_RECURSE RAVEN_HEADERS FOLLOW_SYMLINKS *.h)
FILE(GLOB_RECURSE RAVEN_CPPS FOLLOW_SYMLINKS *.cpp)
 
find_package(ZLIB CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(xxhash CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)
 
if(NOT WIN32)
	find_library(DLPATH NAMES libdl.so dl REQUIRED)
endif()

set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "RavenDB Client Directory")

set(LibSources ${RAVEN_HEADERS} ${RAVEN_CPPS})

option(BUILD_SHARED_LIBS "Build RavenDB Client as shared library" OFF)

if(BUILD_SHARED_LIBS)
	add_library(Raven_CppClient SHARED ${LibSources})
else()
add_library(Raven_CppClient STATIC ${LibSources})
endif()
 

set_target_properties(Raven_CppClient PROPERTIES OUTPUT_NAME Raven.CppClient) 
 
if(WIN32)
	target_link_libraries(Raven_CppClient ZLIB::ZLIB nlohmann_json nlohmann_json::nlohmann_json CURL::libcurl)
else()
	target_link_libraries(Raven_CppClient ZLIB::ZLIB nlohmann_json nlohmann_json::nlohmann_json CURL::libcurl dl)
endif()
 
if(WIN32)
	target_link_libraries(Raven_CppClient "Ws2_32.lib")
	target_link_libraries(Raven_CppClient "Wldap32.lib")
	target_link_libraries(Raven_CppClient "Crypt32.lib")
	target_link_libraries(Raven_CppClient "Rpcrt4.lib")	
endif()

target_compile_definitions(Raven_CppClient PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")