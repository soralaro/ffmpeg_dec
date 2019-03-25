cmake_minimum_required(VERSION 3.5)

project(ffmpeg_yuv)
set(CMAKE_CXX_STANDARD 11)


set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -fpermissive -fno-strict-aliasing -fopenmp")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall  -fopenmp")

set (TARGET_NAME "main")

file(GLOB_RECURSE cpp_files "*.cpp")
#add_executable(${TARGET_NAME} ${cpp_files})
add_executable(${TARGET_NAME} "main.cpp")

target_link_libraries(${TARGET_NAME} PRIVATE -lavcodec -lavformat -lavutil -lswscale)