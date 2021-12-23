#include <pybind11/pybind11.h>
#include "Engine.h"
#include "checkError.h"
#include <glad/glad.h>

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

std::unique_ptr<Engine> createEngine(int logicalWidth, int logicalHeight,
                                                   int physicalWidth, int physicalHeight,
                                                   float contentScaleX, float contentScaleY) {
    gladLoadGL();
    SurfaceInfo si {
            .logicalWidth = logicalWidth,
            .logicalHeight = logicalHeight,
            .physicalWidth = physicalWidth,
            .physicalHeight = physicalHeight,
            .contentScaleX = contentScaleX,
            .contentScaleY = contentScaleY,
    };
    return std::make_unique<Engine>(si);
}

PYBIND11_MODULE(binding_test, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    py::class_<Engine>(m, "Engine")
            .def(py::init(&createEngine))
            .def("render", &Engine::render)
            .def("setRandomGlobalDiffuse", &Engine::setRandomGlobalDiffuse);

    m.def("add", &add, "A function which adds two numbers");
}
