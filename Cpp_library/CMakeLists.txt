# Cpp_OpenGL/Cpp_library/CMakeLists.txt
add_library(cpp_core)

# Your core sources
target_sources(cpp_core PRIVATE
  src/engine.cpp
  third_party/glad/src/glad.c      # compile glad once, inside core
)

# Public includes for your API, private for glad
target_include_directories(cpp_core
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
)


# turn off glm’s tests/examples
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glm EXCLUDE_FROM_ALL)
target_link_libraries(cpp_core PRIVATE glm::glm)


# GLFW from submodule
add_subdirectory(third_party/glfw)

# Link glfw privately (don’t leak headers in public API)
target_link_libraries(cpp_core PRIVATE glfw)

# On some Linux setups, GLFW’s CMake will pull in needed system libs (X11, dl, pthread).