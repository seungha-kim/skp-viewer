#include <pybind11/pybind11.h>
#include "Engine.h"
#include "checkError.h"
#include <glad/glad.h>
#include "KeyCommandSet.h"

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

    py::enum_<KeyCommand>(m, "KeyCommand")
            .value("EXIT", KeyCommand::EXIT)
            .value("FLY_MODE_TOGGLE", KeyCommand::FLY_MODE_TOGGLE)
            .value("FLY_MODE_FORWARD", KeyCommand::FLY_MODE_FORWARD)
            .value("FLY_MODE_BACKWARD", KeyCommand::FLY_MODE_BACKWARD)
            .value("FLY_MODE_LEFT", KeyCommand::FLY_MODE_LEFT)
            .value("FLY_MODE_RIGHT", KeyCommand::FLY_MODE_RIGHT)
            .value("FLY_MODE_UPWARD", KeyCommand::FLY_MODE_UPWARD)
            .value("FLY_MODE_DOWNWARD", KeyCommand::FLY_MODE_DOWNWARD);

    py::class_<KeyCommandSet>(m, "KeyCommandSet")
            .def(py::init<>())
            .def("setPressed", &KeyCommandSet::setPressed)
            .def("getPressed", &KeyCommandSet::getPressed);

    py::class_<Engine>(m, "Engine")
            .def(py::init<SurfaceInfo>())
            .def("render", &Engine::render)
            .def("handleInput", &Engine::handleInput)
            .def("updateTime", &Engine::updateTime)
            .def("onKeyboardStateChange", &Engine::onKeyboardStateChange)
            .def("shouldContinuouslyRender", &Engine::shouldContinuouslyRender)
            .def("resize", &Engine::resize)
            .def("setRandomGlobalDiffuse", &Engine::setRandomGlobalDiffuse);

    m.def("add", &add, "A function which adds two numbers");
}
