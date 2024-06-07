cmake_minimum_required(VERSION 3.10)
project(test_run)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# 使用 file(GLOB ...) 命令获取 src 目录下所有的 .cpp 文件
file(GLOB SOURCES "./src/*.cpp")

# 添加可执行文件并包含所有的 .cpp 文件
add_executable(TS_VNS ${SOURCES})

#add_executable(test_run src/test.cpp)
#add_executable(test_run src/main.cpp)


