cmake_minimum_required(VERSION 2.8)
project(vecadd)


#find_package(OpenCL REQUIRED)

set(SRC vecadd.cpp)
add_executable(vecadd ${SRC})
target_link_libraries(vecadd OpenCL)
