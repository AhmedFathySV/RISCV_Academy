# 1. Gather source files
file(GLOB_RECURSE BOX_FILTER_SOURCES src/*.cpp)

# 2. Create a library for the box filter logic
add_library(box_filter_lib STATIC ${BOX_FILTER_SOURCES})

target_include_directories(box_filter_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/include
)

set_target_properties(box_filter_lib PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)

# 3. Create the test executable
add_executable(box_filter_test test/box_filter_test.cpp)
target_link_libraries(box_filter_test PRIVATE box_filter_lib ${CMAKE_PROJECT_NAME}_qemu)

# 4. Custom target to run it
add_custom_target(run_box_filter_qemu
    COMMAND ${QEMU_BINARY} -L /usr/riscv64-linux-gnu box_filter_test
    DEPENDS box_filter_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)