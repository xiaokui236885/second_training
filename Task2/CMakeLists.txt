cmake_minimum_required(VERSION 3.16)
project(task2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(task2 Task2.cpp)
target_link_libraries(task2 ${OpenCV_LIBS})