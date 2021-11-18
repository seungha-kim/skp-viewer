#include "Window.h"
#include "Gui.h"
#include "ProgramSelector.h"
#include "graphics/Material.h"
#include <glm/glm.hpp>

const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

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
        auto& program = selector.currentProgram();

        window.processKeyboardInput();

        window.updateTime();
        window.updateCamera();

        RenderContext renderCtx {
            .scene = window.sceneManager().activeScene(),
            .playbackValue = window.playbackValue(),
            .globalMaterial = globalMaterial,
            .windowDimension = window.dimension(),
        };
        program.render(renderCtx);

        GuiContext guiCtx {
                .programSelector = selector,
                .sceneManager = window.sceneManagerMut(),
                .playbackState = window.playbackStateMut(),
                .inputController = window.inputControllerMut(),
                .globalMaterial = globalMaterial,
                .windowDimension = window.dimension(),
        };
        program.processGui(guiCtx);

        // Many mutation can happen here, so this call must be located in the end.
        gui.process(guiCtx);

        window.swapBuffers();
        window.waitEvents();
    }
    return 0;
}

