cmake_minimum_required(VERSION 3.8)
include(Project.cmake)
project(${CMAKE_PROJECT_NAME} VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(NDEBUG false CACHE STRING "Set to true to disable debug related items such as assertions")
add_definitions(-DNDEBUG=${NDEBUG})
message("NDEBUG=${NDEBUG}")

set(source_dir "${PROJECT_SOURCE_DIR}")

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

file(GLOB SOURCE_COMMON "*.cc")
file(GLOB HEADERS_COMMON "*.h")

set(PROJECT_SOURCE ${SOURCE_COMMON})
set(PROJECT_HEADERS ${HEADERS_COMMON})

# add_executable(${CMAKE_PROJECT_NAME} "main.cpp" "main.h" ${PROJECT_SOURCE} ${PROJECT_HEADERS})
add_library(${CMAKE_PROJECT_NAME} MODULE ${PROJECT_SOURCE} ${PROJECT_HEADERS})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".dll")

set(THREADS_PREFER_PTHREAD_FLAG ON)


#tester dll
add_library(tester MODULE "tester/tester.cc")
set_target_properties(tester PROPERTIES PREFIX "csp_")
set_target_properties(tester PROPERTIES SUFFIX ".dll")

#timemanager dll
add_library(timemanager MODULE "lib/utils/time.h" "lib/utils/timemanager.h" "lib/utils/timemanager.cc")
set_target_properties(timemanager PROPERTIES PREFIX "csp_")
set_target_properties(timemanager PROPERTIES SUFFIX ".dll")