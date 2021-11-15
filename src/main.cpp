#include "Window.h"
#include "Gui.h"
#include "ProgramSelector.h"
#include "graphics/Material.h"
#include <glm/glm.hpp>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    Window::initGlfw();
    Window window{SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL"};
    window.bind();
    Window::initGl();

    ProgramSelector selector;
    auto gui = Gui(window);

    Material globalMaterial = {
            .ambient = glm::vec3(0.1f, 0.1f, 0.1f),
            .diffuse = glm::vec3(0.3f, 0.2f, 0.1f),
            .specular = glm::vec3(1.0f, 1.0f, 1.0f),
            .shininess = 32.0f,
    };

    while (!window.shouldClose())
    {
        window.updateTime();
        window.processKeyboardInput();

        RenderContext renderCtx {
            .cam = window.cameraManager().activeCamera(),
            .playbackValue = window.playbackValue(),
            .globalMaterial = globalMaterial,
        };
        selector.renderProgram(renderCtx);

        GuiContext guiCtx {
                .programSelector = selector,
                .cameraManager = window.cameraManagerMut(),
                .playbackState = window.playbackStateMut(),
                .inputController = window.inputControllerMut(),
                .globalMaterial = globalMaterial,
        };
        gui.process(guiCtx);

        window.swapBuffers();
        window.waitEvents();
    }
    return 0;
}

