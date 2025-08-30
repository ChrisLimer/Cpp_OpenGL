#pragma once
#include <vector>
#include <cstdint>


namespace cpp_core {
struct Mesh {
std::vector<float> positions; // xyz packed, size = 3*vertex_count
std::vector<uint32_t> indices; // triangle indices (3*k)
};


struct RGBA { float r=1, g=1, b=1, a=1; };
}