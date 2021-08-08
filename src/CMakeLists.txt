cmake_minimum_required(VERSION 3.8)
include(Project.cmake)
project(${CMAKE_PROJECT_NAME} VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
	
include(CheckIncludeFileCXX)
check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE) 
		set(CMAKE_CXX_STANDARD 20)
		message("cpp 20 detected")
elseif (HAS_ANY AND HAS_STRING_VIEW)
		set(CMAKE_CXX_STANDARD 17)
		message("cpp 17 detected")
else()
		set(CMAKE_CXX_STANDARD 14)
		message("cpp 14 detected")
endif()

if (WIN32)
	message("CMAKE DETECTED WINDOWS")
	# add_compile_options(/W4)
	set_directory_properties(PROPERTIES
			VS_STARTUP_PROJECT ${PROJECT_NAME}
	)
else()
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

set(NDEBUG false CACHE STRING "Set to true to disable debug related items such as assertions")
add_definitions(-DNDEBUG=${NDEBUG})
message("NDEBUG=${NDEBUG}")

set(source_dir "${PROJECT_SOURCE_DIR}")

file(GLOB SOURCE_COMMON "*.cc")
file(GLOB HEADERS_COMMON "*.h")

set(PROJECT_SOURCE ${SOURCE_COMMON})
set(PROJECT_HEADERS ${HEADERS_COMMON})

# add_executable(${CMAKE_PROJECT_NAME} "main.cpp" "main.h" ${PROJECT_SOURCE} ${PROJECT_HEADERS})
add_library(${CMAKE_PROJECT_NAME} MODULE ${PROJECT_SOURCE} ${PROJECT_HEADERS})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".dll")

set(THREADS_PREFER_PTHREAD_FLAG ON)

if (WIN32)
	set(PROJECT_TESTER tester_win32)
	set(PROJECT_TIMEMANAGER timemanager_win32)
else()
	set(PROJECT_TESTER tester)
	set(PROJECT_TIMEMANAGER timemanager)
endif()

#tester dll
add_library(${PROJECT_TESTER} MODULE "tester/tester.cc")
set_target_properties(${PROJECT_TESTER} PROPERTIES PREFIX "csp_")
set_target_properties(${PROJECT_TESTER} PROPERTIES SUFFIX ".dll")

#timemanager dll
add_library(${PROJECT_TIMEMANAGER} MODULE
 "lib/utils/time.h"
 "lib/utils/timemanager_ext.cc"
 "lib/utils/timemanager.h"
 "lib/utils/timemanager.cc"
)
set_target_properties(${PROJECT_TIMEMANAGER} PROPERTIES PREFIX "csp_")
set_target_properties(${PROJECT_TIMEMANAGER} PROPERTIES SUFFIX ".dll")