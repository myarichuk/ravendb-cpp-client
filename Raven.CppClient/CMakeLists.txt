cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB RAVEN_HEADERS ./*.h)
FILE(GLOB RAVEN_CPPS ./*.cpp)

find_package(ZLIB REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(CURL REQUIRED)

add_library(Raven_CppClient SHARED ${RAVEN_HEADERS} ${RAVEN_CPPS})

target_link_libraries(Raven_CppClient ZLIB::ZLIB)
target_link_libraries(Raven_CppClient nlohmann_json nlohmann_json::nlohmann_json)
 
 if(NOT ${CURL_LIBRARIES} STREQUAL "")
	message("Found Curl libaries at ${CURL_LIBRARIES}")
 endif()

 if(NOT ${CURL_INCLUDE_DIRS} STREQUAL "")
	message("Found Curl includes at ${CURL_INCLUDE_DIRS}")
endif()

target_link_libraries(Raven_CppClient ${CURL_LIBRARIES})
include_directories(${CURL_INCLUDE_DIRS})

if(WIN32)
	target_link_libraries(Raven_CppClient "Ws2_32.lib")
	target_link_libraries(Raven_CppClient "Wldap32.lib")
	target_link_libraries(Raven_CppClient "Crypt32.lib")
	target_link_libraries(Raven_CppClient "${CMAKE_LIBRARY_PATH}/libcurl.lib")
endif()

target_link_libraries(Raven_CppClient OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(Raven_CppClient PUBLIC . .. ../external)
 
target_compile_definitions(Raven_CppClient PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")