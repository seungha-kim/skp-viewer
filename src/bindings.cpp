#include <pybind11/pybind11.h>
#include "Engine.h"
#include "checkError.h"
#include <glad/glad.h>

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

void init() {
    gladLoadGL();
}

PYBIND11_MODULE(binding_test, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("init", &init, "initialize OpenGL");

    py::class_<SurfaceInfo>(m, "SurfaceInfo")
            .def(py::init([](
                    int logicalWidth, int logicalHeight,
                    int physicalWidth, int physicalHeight,
                    float contentScaleX, float contentScaleY
                    ) {
                SurfaceInfo value = {
                        .logicalWidth = logicalWidth,
                        .logicalHeight = logicalHeight,
                        .physicalWidth = physicalWidth,
                        .physicalHeight = physicalHeight,
                        .contentScaleX = contentScaleX,
                        .contentScaleY = contentScaleY,
                };
                return std::make_unique<SurfaceInfo>(value);
            }));

    py::class_<Engine>(m, "Engine")
            .def(py::init<SurfaceInfo>())
            .def("render", &Engine::render)
            .def("resize", &Engine::resize)
            .def("setRandomGlobalDiffuse", &Engine::setRandomGlobalDiffuse);

    m.def("add", &add, "A function which adds two numbers");
}
