cmake_minimum_required(VERSION 3.16)     #设定CMake最低版本要求为3.16,低于这个版本会报错

project(flower_opencv LANGUAGES CXX)     #项目名称为flower_opencv,指定使用C++语言

set(CMAKE_CXX_STANDARD 17)     #设定C++标准为17
set(CMAKE_CXX_STANDARD_REQUIRED ON)     #强制要求编译器必须支持C++17标准,否则报错

find_package(OpenCV REQUIRED)     #找到OpenCV库(必须写在add_executable前面)

add_executable(flower flower.cpp)     #新增flower程序

target_link_libraries(flower ${OpenCV_LIBS})     #给flower连接OpenCV库,让它能调用imread,imshow等函数