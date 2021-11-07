#include "Window.h"
#include "Gui.h"
#include "Renderer.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    Window::initGlfw();
    Window window{SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL"};
    window.bind();
    Window::initGl();

    auto gui = Gui(window);
    Renderer renderer;

    while (!window.shouldClose())
    {
        window.updateTime();
        window.processKeyboardInput();
        renderer.render(window.cameraManager().activeCamera(), window.playbackValue());

        GuiRenderContext guiCtx {
                .cameraManager = window.cameraManager(),
                .playbackState = window.playbackState(),
                .inputController = window.inputController(),
        };
        gui.render(guiCtx);

        window.swapBuffers();
        window.waitEvents();
    }
    return 0;
}

