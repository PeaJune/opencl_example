cmake_minimum_required(VERSION 2.8)
project(cuda_vecadd)

FIND_PACKAGE(CUDA REQUIRED)

set(SRC cuda_vecadd.cu)
cuda_add_executable(cuda_vecadd ${SRC})
target_link_libraries(cuda_vecadd )
