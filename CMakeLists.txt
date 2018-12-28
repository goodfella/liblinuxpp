cmake_minimum_required(VERSION 3.5.1)
project(liblinuxpp CXX)

set (CMAKE_EXPORT_COMPILE_COMMANDS On)

add_library(linuxpp SHARED
  src/pipe.cpp
  src/open.cpp
  src/epoll.cpp
  src/subprocess/wait.cpp
  src/subprocess/status.cpp
  src/subprocess/stream.cpp
  src/subprocess/pipe_stream.cpp
  src/subprocess/path_stream.cpp
  src/subprocess/null_stream.cpp
  src/subprocess/fd_stream.cpp
  src/subprocess/stream_descriptors.cpp
  src/subprocess/argv.cpp
  src/subprocess/popen.cpp
  src/net/socket.cpp
  src/net/bind.cpp
  src/net/sockaddr.cpp
  src/net/send.cpp
  src/net/recv.cpp
  src/net/sockopt.cpp
  src/net/udp_socket.cpp)

target_include_directories(linuxpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(linuxpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(linuxpp INTERFACE -std=c++14)
target_compile_options(linuxpp PRIVATE -std=c++14 -pedantic -Wall -Werror)
target_link_libraries(linuxpp ndgpp)

set(liblinuxpp_compiler_flags -pedantic -Wall -Werror)

if (LIBLINUXPP_UNIT_TESTS)
  add_subdirectory(test)
endif()
