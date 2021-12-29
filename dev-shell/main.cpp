#include "Window.h"

const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

int main()
{
    Window window{SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL"};
    window.mainLoop();


    return 0;
}

