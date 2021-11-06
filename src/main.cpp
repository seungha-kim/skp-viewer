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
        window.processInput();
        renderer.render(window.cameraState(), window.playbackValue());

        GuiRenderContext guiCtx {
                .cameraState = window.cameraState(),
                .playbackState = window.playbackState(),
                .inputState = window.inputState(),
        };
        gui.render(guiCtx);

        window.swapBuffers();
        window.waitEvents();
    }
    return 0;
}

