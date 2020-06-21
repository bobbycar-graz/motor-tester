cmake_minimum_required(VERSION 3.10)

project(motor-tester)

add_executable(motor-tester main.cpp)

target_include_directories(motor-tester PUBLIC "${PROJECT_SOURCE_DIR}/bobbycar-protocol")

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
