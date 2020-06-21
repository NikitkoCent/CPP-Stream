cmake_minimum_required(VERSION 3.8.2)
project(cpp-stream CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third-party)

set(_cpp-stream_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(cpp-stream_INCLUDE_DIR ${_cpp-stream_INCLUDE_DIR} PARENT_SCOPE)

include_directories(${_cpp-stream_INCLUDE_DIR})

# Enable testing for the project
enable_testing()
add_subdirectory(tests)
