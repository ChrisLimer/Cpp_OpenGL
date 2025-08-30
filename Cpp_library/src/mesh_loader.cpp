#include "cpp_core/mesh.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>
#include <stdexcept>
#include <string>

namespace cpp_core {

// Build a Mesh using only position indices (ignores normals/uvs for now)
static Mesh load_obj_positions_only(const std::string& path) {
    tinyobj::attrib_t attrib; std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials; std::string warn, err;
    tinyobj::ObjReaderConfig cfg; cfg.triangulate = true;

    bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), nullptr, cfg.triangulate);
    if (!ok) throw std::runtime_error("tinyobjloader: " + warn + err);

    Mesh mesh; mesh.positions.clear(); mesh.indices.clear();
    mesh.positions.reserve(attrib.vertices.size());

    // map original vertex index -> compact index
    std::unordered_map<int, uint32_t> remap; remap.reserve(attrib.vertices.size()/3);

    for (const auto& sh : shapes) {
        for (const auto& idx : sh.mesh.indices) {
            int vi = idx.vertex_index; // index into attrib.vertices (triplets)
            auto it = remap.find(vi);
            uint32_t new_i;
            if (it == remap.end()) {
                new_i = static_cast<uint32_t>(mesh.positions.size()/3);
                remap.emplace(vi, new_i);
                // copy xyz
                mesh.positions.push_back(attrib.vertices[3*vi + 0]);
                mesh.positions.push_back(attrib.vertices[3*vi + 1]);
                mesh.positions.push_back(attrib.vertices[3*vi + 2]);
            } else {
                new_i = it->second;
            }
            mesh.indices.push_back(new_i);
        }
    }

    return mesh;
}

} // namespace cpp_core