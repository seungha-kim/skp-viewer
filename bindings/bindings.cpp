#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "../engine/Engine.h"
#include "../engine/render/checkError.h"
#include <glad/glad.h>
#include "../engine/reader/AbstractReader.h"
#include "../engine/reader/AssimpReader.h"
#include "../engine/reader/SketchupReader.h"

namespace py = pybind11;

void init() {
    static bool initialized = false;
    if (initialized) return;
    gladLoadGL();
    initialized = true;
}

PYBIND11_MODULE(binding_test, m) {
    using namespace acon;
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
            .def(py::init<const AbstractReader&>())
            .def("render", [](Engine& engine, float playbackValue){
                engine.render(playbackValue, {});
            })
            .def("prepareToRender", &Engine::prepareToRender)
            .def("resize", &Engine::resize)
            .def("currentCameraStateMut", &Engine::currentCameraStateMut, py::return_value_policy::reference)
            .def("runtimeModelMut", &Engine::runtimeModelMut, py::return_value_policy::reference);

    py::class_<RuntimeModel>(m, "RuntimeModel")
            .def("getTagCount", &RuntimeModel::getTagCount)
            .def("getTag", &RuntimeModel::getTag)
            .def("getTagName", &RuntimeModel::getTagName)
            .def("getTagVisibility", &RuntimeModel::getTagVisibility)
            .def("setTagVisibility", &RuntimeModel::setTagVisibility);

    py::class_<AbstractReader>(m, "AbstractReader");

    py::class_<AssimpReader, AbstractReader>(m, "AssimpReader")
            .def(py::init<std::string_view>());

    py::class_<SketchupReader, AbstractReader>(m, "SketchupReader")
            .def(py::init<std::string_view>());

    py::class_<CameraState>(m, "CameraState")
            .def(py::init<>())
            .def("front", &CameraState::front)
            .def("left", &CameraState::left)
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
            .def(py::self + py::self)
            .def(py::self - py::self)
            .def(-py::self)
            .def(float() * py::self)
            .def(py::self * float())
            .def_readwrite("x", &glm::vec3::x)
            .def_readwrite("y", &glm::vec3::y)
            .def_readwrite("z", &glm::vec3::z);
}
