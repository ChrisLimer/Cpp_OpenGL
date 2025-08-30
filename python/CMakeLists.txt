# Cpp_OpenGL/python/CMakeLists.txt
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

find_package(pybind11 CONFIG QUIET)
if (NOT pybind11_FOUND)
  include(FetchContent)
  FetchContent_Declare(pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.13.6
  )
  FetchContent_MakeAvailable(pybind11)
endif()

pybind11_add_module(pycpp MODULE pybind_module.cpp)
target_link_libraries(pycpp PRIVATE cpp_core)

set_target_properties(pycpp PROPERTIES
  CXX_STANDARD 20
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
)
