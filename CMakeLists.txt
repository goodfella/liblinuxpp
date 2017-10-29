cmake_minimum_required(VERSION 3.5.1)
project(liblinuxpp CXX)

add_library(linuxpp INTERFACE)
target_include_directories(linuxpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(linuxpp INTERFACE -std=c++14)

enable_testing()

set(project_compiler_flags -pedantic -Wall)

include_directories(googletest/include)

add_subdirectory(googletest)
add_subdirectory(test)
