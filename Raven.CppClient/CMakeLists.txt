cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB RAVEN_HEADERS ./*.h)
FILE(GLOB RAVEN_CPPS ./*.cpp)

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS
	  "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:library")
endif()

find_package(ZLIB REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(CURL REQUIRED)

set(LibSources ${RAVEN_HEADERS} ${RAVEN_CPPS})

add_library(Raven_CppClient OBJECT  ${LibSources})

set_property(TARGET Raven_CppClient PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(Raven_CppClient_shared SHARED $<TARGET_OBJECTS:Raven_CppClient>)
add_library(Raven_CppClient_static STATIC $<TARGET_OBJECTS:Raven_CppClient>)

target_link_libraries(Raven_CppClient_shared ZLIB::ZLIB)
target_link_libraries(Raven_CppClient_static ZLIB::ZLIB)
target_link_libraries(Raven_CppClient_shared nlohmann_json nlohmann_json::nlohmann_json)
target_link_libraries(Raven_CppClient_static nlohmann_json nlohmann_json::nlohmann_json)
 
 if(NOT CURL_LIBRARIES STREQUAL "")
	message("Found Curl libaries at ${CURL_LIBRARIES}")
 endif()

 if(NOT CURL_INCLUDE_DIRS STREQUAL "")
	message("Found Curl includes at ${CURL_INCLUDE_DIRS}")
endif()

target_link_libraries(Raven_CppClient_static ${CURL_LIBRARIES})
target_link_libraries(Raven_CppClient_shared ${CURL_LIBRARIES})
include_directories(${CURL_INCLUDE_DIRS})

if(WIN32)
	target_link_libraries(Raven_CppClient_shared "Ws2_32.lib")
	target_link_libraries(Raven_CppClient_shared "Wldap32.lib")
	target_link_libraries(Raven_CppClient_shared "Crypt32.lib")

	target_link_libraries(Raven_CppClient_static "Ws2_32.lib")
	target_link_libraries(Raven_CppClient_static "Wldap32.lib")
	target_link_libraries(Raven_CppClient_static "Crypt32.lib")
endif()

target_link_libraries(Raven_CppClient_shared OpenSSL::SSL OpenSSL::Crypto)
include_directories(. .. ../external)

target_link_libraries(Raven_CppClient_shared OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(Raven_CppClient_static PUBLIC . .. ../external)
 
target_compile_definitions(Raven_CppClient_shared PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")
target_compile_definitions(Raven_CppClient_static PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")