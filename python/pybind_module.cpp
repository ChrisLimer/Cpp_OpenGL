// Cpp_OpenGL/python/pybind_module.cpp
// #include <stdio>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>
#include <span>
#include "cpp_core/engine.hpp"

namespace py = pybind11;
using cpp_core::Camera; using cpp_core::RenderConfig; using cpp_core::Vec3;

static void render_scatter_np(const py::array_t<float, py::array::c_style | py::array::forcecast>& pts,
                              Vec3 eye, Vec3 center, Vec3 up,
                              int width=1280, int height=720,
                              float point_size=4.0f,
                              py::tuple color = py::make_tuple(1.0f,1.0f,1.0f),
                              py::tuple bgcolor = py::make_tuple(0.05f,0.05f,0.08f))
{
    // Expect shape (N, 3)
    std::cout << "pts.ndim(): " << pts.ndim() << std::endl;
    std::cout << "pts.shape(): " << pts.shape(1) << std::endl;
    if (pts.ndim() != 2 || pts.shape(1) != 3)
    {
        std::cout << "points must be shaped (N,3) float32" << std::endl;
        throw std::runtime_error("points must be shaped (N,3) float32");
    }
    else
    {
        std::cout << "points has shaped (N,3) float32" << std::endl;
    }

    auto buf = pts.request();
    const float* data = static_cast<const float*>(buf.ptr);
    size_t n = static_cast<size_t>(pts.shape(0));
    std::span<const float> span(data, n*3);

    Camera cam; cam.eye=eye; cam.center=center; cam.up=up;

    RenderConfig cfg; cfg.width=width; cfg.height=height; cfg.point_size=point_size;
    cfg.color_r = color.size()>0 ? color[0].cast<float>() : 1.0f;
    cfg.color_g = color.size()>1 ? color[1].cast<float>() : 1.0f;
    cfg.color_b = color.size()>2 ? color[2].cast<float>() : 1.0f;
    cfg.bg_r = bgcolor.size()>0 ? bgcolor[0].cast<float>() : 0.05f;
    cfg.bg_g = bgcolor.size()>1 ? bgcolor[1].cast<float>() : 0.05f;
    cfg.bg_b = bgcolor.size()>2 ? bgcolor[2].cast<float>() : 0.08f;

    cpp_core::render_scatter(span, cam, cfg);
}

PYBIND11_MODULE(pycpp, m){
    m.doc() = "pybind11 bindings for Cpp_OpenGL scatter renderer";

    py::class_<Vec3>(m, "Vec3")
        .def(py::init<float,float,float>())
        .def_readwrite("x", &Vec3::x)
        .def_readwrite("y", &Vec3::y)
        .def_readwrite("z", &Vec3::z);

    py::class_<Camera>(m, "Camera")
        .def(py::init<>())
        .def_readwrite("eye", &Camera::eye)
        .def_readwrite("center", &Camera::center)
        .def_readwrite("up", &Camera::up)
        .def_readwrite("fov_deg", &Camera::fov_deg)
        .def_readwrite("near_plane", &Camera::near_plane)
        .def_readwrite("far_plane", &Camera::far_plane);

    m.def("render_scatter", &render_scatter_np,
          py::arg("points"),
          py::arg("eye"), py::arg("center"), py::arg("up"),
          py::arg("width")=1280, py::arg("height")=720,
          py::arg("point_size")=4.0f,
          py::arg("color")=py::make_tuple(1.0f,1.0f,1.0f),
          py::arg("bgcolor")=py::make_tuple(0.05f,0.05f,0.08f),
          R"doc( Render a 3D scatter plot from NumPy (N,3) float32.
                - eye, center, up are Vec3 (or 3-tuples convertible to Vec3)
                - window stays open until you close it or press ESC )doc");
}