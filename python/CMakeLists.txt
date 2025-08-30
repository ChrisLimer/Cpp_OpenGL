# python/CMakeLists.txt
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
# Use scikit-build-core’s bundled pybind if you like; or vendor pybind11:
find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
  message(STATUS "Using pybind11 via FetchContent")
  include(FetchContent)
  FetchContent_Declare(pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.13.6
  )
  FetchContent_MakeAvailable(pybind11)
endif()

pybind11_add_module(pycpp MODULE pybind_module.cpp)
target_link_libraries(pycpp PRIVATE cpp_core)
target_compile_definitions(pycpp PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)
