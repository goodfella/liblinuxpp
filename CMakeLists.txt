cmake_minimum_required(VERSION 3.5.1)
project(liblinuxpp CXX)

add_library(linuxpp SHARED
  src/pipe.cpp
  src/open.cpp)

target_include_directories(linuxpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(linuxpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(linuxpp INTERFACE -std=c++14)
target_compile_options(linuxpp PRIVATE -std=c++14 -pedantic -Wall -Werror)
target_link_libraries(linuxpp ndgpp)

set(project_compiler_flags -pedantic -Wall)

if (LIBLINUXPP_UNIT_TESTS)
  add_subdirectory(test)
endif()
