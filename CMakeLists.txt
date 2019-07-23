cmake_minimum_required(VERSION 3.13)
project(raven_cpp_client)

set(CMAKE_CXX_STANDARD 17)

#library names for vcpkg to install
set(PROJECT_DEPENDENCIES xxhash curl nlohmann-json openssl gtest zlib)

if(NOT DEFINED ARCH)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	  MESSAGE("++ 64 bit architecture") 
	  set(ARCH "x64") 
	 else() 
	  MESSAGE("++ 32 bit architecture") 
	  set(ARCH "x86") 
	endif()
endif() 
  
if(WIN32)
#this is most likely due to a bug in vcpkg configuration, we need to set these manually...
	set(VCPKG_TARGET_TRIPLET "${ARCH}-windows-static")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
	include(FindWindowsSDK)

	if(WINDOWSSDK_FOUND)
		foreach(_sdk ${WINDOWSSDK_DIRS})
			get_windowssdk_library_dirs("${_sdk}" _dirs)	
			message(STATUS "WinSDK libs: ${_dirs}")
			foreach(_dir ${_dirs})
				link_directories(${_dir})
			endforeach()
		endforeach()		
	else()
		message(FATAL_ERROR "Couldn't find Windows SDK folder, cannot compile RavenDB C++ Client without it.")
	endif()
elseif(UNIX)
	set(VCPKG_TARGET_TRIPLET "${ARCH}-linux-static")
elseif(APPLE)
	set(VCPKG_TARGET_TRIPLET "${ARCH}-osx-static")
else()
#TODO: add arm triplets too, test the builds on arm dockers
	message(FATAL_ERROR "Can compile only on Unix, Windows or MacOS operating systems.")
endif()

message("Build target: ${VCPKG_TARGET_TRIPLET}")

#make sure to install and build vcpkg to enable them for the solution
if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})

#first find proper path for vcpkg
  if(NOT DEFINED ENV{VCPKG_ROOT})
	if(WIN32)	
	  set(VCPKG_ROOT $ENV{HOMEDRIVE}$ENV{HOMEPATH}vcpkg)
	else()
	  set(VCPKG_ROOT $ENV{HOME}/.vcpkg)
	endif()
  else()
	set(VCPKG_ROOT $ENV{VCPKG_ROOT})
  endif()

#then make sure vcpkg is up to date
  if(NOT EXISTS ${VCPKG_ROOT})
	message("Cloning vcpkg in ${VCPKG_ROOT}")
	execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
	# If a reproducible build is desired (and potentially old libraries are # ok), uncomment the
	# following line and pin the vcpkg repository to a specific githash.
	# execute_process(COMMAND git checkout 745a0aea597771a580d0b0f4886ea1e3a94dbca6 WORKING_DIRECTORY ${VCPKG_ROOT})
  else()
	# The following command has no effect if the vcpkg repository is in a detached head state.
	message("Auto-updating vcpkg in ${VCPKG_ROOT}")
	execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
	execute_process(COMMAND ${VCPKG_EXEC} update)
  endif()

  if(NOT EXISTS ${VCPKG_ROOT}/README.md)
	message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg, make sure git is installed and configured properly *****")
  endif()

  #compile vcpkg if this is a fresh installation
  if(WIN32)
	set(BOOST_INCLUDEDIR ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include)
	set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
	set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
  else()
	set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
	set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
	message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
	execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
	message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
  endif()

  #make sure to include third party stuff with CMake compilation
  set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

  message(STATUS "***** Checking project third party dependencies in ${VCPKG_ROOT} *****")

  #actually install vcpkg libraries
  execute_process(COMMAND ${VCPKG_EXEC} install --triplet ${VCPKG_TARGET_TRIPLET} ${PROJECT_DEPENDENCIES} WORKING_DIRECTORY ${VCPKG_ROOT})
endif()

message("Share path: ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share")
message("Include path: ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")
message("Lib path: ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
  
set(CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share")
set(CMAKE_INCLUDE_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")
set(CMAKE_LIBRARY_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")

enable_testing() 

add_subdirectory(Raven.CppClient)

#choose if buildings tryouts
option(BUILD_TRYOUTS "Build the Tryouts" OFF)
if(BUILD_TRYOUTS)
	add_subdirectory(Tryouts)
endif()
  
#choose if building tests
option(BUILD_TESTS "Build the Tests" OFF)
if(BUILD_TESTS)
	add_subdirectory(Raven.CppClient.Tests)
endif()
  
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
 
FILE(GLOB include_files Raven.CppClient/*.h)
FILE(GLOB external_include_files external/*)
FILE(GLOB ca_bundle_files ca_bundle/*)

FILE(GLOB HELLO_WORLD_APP ./hello_world.cpp)

install(TARGETS Raven_CppClient LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
#install(FILES ${include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
#install(FILES ${external_include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/external)
install(FILES ${ca_bundle_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/ca_bundle)

install(FILES ${HELLO_WORLD_APP} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
