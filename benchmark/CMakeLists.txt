cmake_minimum_required(VERSION 3.16)
project(RISCV_RGB2Gray_Benchmark C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GEM5_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../gem5" CACHE PATH "Path to gem5")

set(SCALAR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../test/reference")
set(VECTOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

add_compile_definitions(USE_GEM5_M5)

add_executable(rgb_benchmark
    main_benchmark.cpp
    ${SCALAR_DIR}/rgb_to_gray_scalar.c
    ${VECTOR_DIR}/rgb_to_gray_vector.c
    ${GEM5_DIR}/util/m5/src/riscv/m5op.S
)

target_include_directories(rgb_benchmark PRIVATE
    ${SCALAR_DIR}
    ${VECTOR_DIR}
    ${GEM5_DIR}/include
)