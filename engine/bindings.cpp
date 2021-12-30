#include <pybind11/pybind11.h>
#include "Engine.h"
#include "render/checkError.h"
#include <glad/glad.h>
#include "scene/CameraState.h"

namespace py = pybind11;

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

    py::class_<CameraState>(m, "CameraState")
            .def(py::init<>())
            .def_readwrite("fovyDeg", &CameraState::fovyDeg)
            .def_readwrite("aspectWidth", &CameraState::aspectWidth)
            .def_readwrite("aspectHeight", &CameraState::aspectHeight)
            .def_readwrite("zNear", &CameraState::zNear)
            .def_readwrite("zFar", &CameraState::zFar)
            .def_readwrite("pos", &CameraState::pos)
            .def_readwrite("up", &CameraState::up)
            .def_readwrite("speed", &CameraState::speed)
            .def_readwrite("pitch", &CameraState::pitch)
            .def_readwrite("yaw", &CameraState::yaw);

    py::class_<glm::vec3>(m, "Vec3")
            .def(py::init<>())
            .def_readwrite("x", &glm::vec3::x)
            .def_readwrite("y", &glm::vec3::y)
            .def_readwrite("z", &glm::vec3::z);
}
