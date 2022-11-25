cmake_minimum_required(VERSION 3.23)
project(shared_memory)

set(CMAKE_CXX_STANDARD 17)

add_executable(producer producer.cpp consumer.cpp)
add_executable(consumer consumer.cpp)
