#include "Window.h"
#include "Gui.h"
#include "ProgramSelector.h"

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


    while (!window.shouldClose())
    {
        window.updateTime();
        window.processKeyboardInput();

        RenderContext renderCtx {
            .cam = window.cameraManager().activeCamera(),
            .playbackValue = window.playbackValue(),
        };
        selector.renderProgram(renderCtx);

        GuiContext guiCtx {
                .programSelector = selector,
                .cameraManager = window.cameraManagerMut(),
                .playbackState = window.playbackStateMut(),
                .inputController = window.inputControllerMut(),
        };
        gui.process(guiCtx);

        window.swapBuffers();
        window.waitEvents();
    }
    return 0;
}

