cmake_minimum_required(VERSION 3.24)
project(TinySTL)
add_executable(TinySTL Test/vector_test.cpp)
target_include_directories(TinySTL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# find . -name "*.hpp" -type f | xargs wc -l