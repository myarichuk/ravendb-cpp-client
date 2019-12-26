set (XXHASH_SRC "${PROJECT_SOURCE_DIR}/External/xxhash")

include(FetchContent)
 
FetchContent_Declare(
  xxhash_cpp
  GIT_REPOSITORY "https://github.com/RedSpah/xxhash_cpp.git"
  GIT_TAG        master
  SOURCE_DIR ${XXHASH_SRC}/repository SUBBUILD_DIR ${XXHASH_SRC}/subbuild  BINARY_DIR ${XXHASH_SRC}/binary
  )

FetchContent_GetProperties(xxhash_cpp)
if(NOT xxhash_cpp_POPULATED)  
  FetchContent_Populate(xxhash_cpp)
endif()
 