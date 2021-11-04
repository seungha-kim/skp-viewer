#pragma once

#include <GLFW/glfw3.h>

class Gui {
public:
    explicit Gui(GLFWwindow& window);
    ~Gui();
    void render();
private:
    GLFWwindow& window;
    bool show_demo_window = true;
    bool show_another_window = false;
};
