# Cpp_OpenGL/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Cpp_OpenGL LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


add_subdirectory(Cpp_library)
add_subdirectory(python)