cmake_minimum_required(VERSION 2.8)
project(printinfo)


#find_package(OpenCL REQUIRED)

set(SRC printinfo.cpp)
add_executable(printinfo ${SRC})
target_link_libraries(printinfo OpenCL)
