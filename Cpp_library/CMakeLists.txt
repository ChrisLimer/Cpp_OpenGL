# Cpp_OpenGL/Cpp_library/CMakeLists.txt
add_library(cpp_core)

target_sources(cpp_core PRIVATE
  src/engine.cpp
  src/mesh_loader.cpp
  third_party/glad/src/glad.c
)

target_include_directories(cpp_core
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
          ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
          ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyobjloader
)

# GLFW (submodule)
# Optional: force backends if you don't have Wayland dev packages installed
# set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)
target_link_libraries(cpp_core PRIVATE glfw)
