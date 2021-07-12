cmake_minimum_required(VERSION 3.8)
project(cpp-playground VERSION 0.1.0)

add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)