// Cpp_OpenGL/Cpp_library/include/cpp_core/engine.hpp
#pragma once
#include <cstddef>
#include <span>

#include <string>
#include "cpp_core/mesh.hpp"

namespace cpp_core {

struct Vec3 { float x, y, z; };

struct Camera {
    Vec3 eye{0,0,3};
    Vec3 center{0,0,0};
    Vec3 up{0,1,0};
    float fov_deg = 60.0f;
    float near_plane = 0.1f;
    float far_plane  = 1000.0f;
};

struct RenderConfig {
    int   width  = 1280;
    int   height = 720;
    float point_size = 4.0f;
    float bg_r = 0.05f, bg_g = 0.05f, bg_b = 0.08f;
    float color_r = 1.0f, color_g = 1.0f, color_b = 1.0f;
};

// Load an .obj from disk and render with a translucent color
void render_obj(const std::string& obj_path,
                const Camera& cam,
                const RenderConfig& cfg = {},
                const RGBA& color = RGBA{100.f/255.f, 100.f/255.f, 200.f/255.f, 125.f/255.f});


// Simple one-shot function: opens a window, renders until it’s closed.
// `points` is a contiguous array of length N*3 (xyz, float32).
void render_scatter(std::span<const float> points,
                    const Camera& cam,
                    const RenderConfig& cfg = {});

} // namespace cpp_core