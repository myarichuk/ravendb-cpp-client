cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB RAVEN_HEADERS ./*.h)
FILE(GLOB RAVEN_CPPS ./*.cpp)

add_library(Raven_CppClient SHARED ${RAVEN_HEADERS} ${RAVEN_CPPS})


find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Raven_CppClient PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(CURL REQUIRED)
target_link_libraries(Raven_CppClient PRIVATE ${CURL_LIBRARIES})
target_include_directories(Raven_CppClient PRIVATE ${CURL_INCLUDE_DIRS})

target_include_directories(Raven_CppClient PUBLIC . .. ../external)

target_compile_definitions(Raven_CppClient PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")
