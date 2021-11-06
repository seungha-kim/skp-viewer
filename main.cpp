#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Gui.h"
#include "CameraOption.h"
#include "RenderOption.h"

struct InputContext {
    CameraOption& cameraOption;
    float deltaTime;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, InputContext &ctx);
void loadTexture(unsigned textureId, const char *filename, unsigned glFormat);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float pitch = 0.0f;
float yaw = -90.0f;
bool cameraRotateMode = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    auto gui = Gui(*window);

    Shader ourShader("simple.vert", "simple.frag");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    };

    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,
            1, 2, 3,
            4, 5, 7,
            5, 6, 7,
            8, 9, 11,
            9, 10, 11,
            12, 13, 15,
            13, 14, 15,
            16, 17, 19,
            17, 18, 19,
            20, 21, 23,
            21, 22, 23,
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    unsigned texture1;
    glGenTextures(1, &texture1);
    loadTexture(texture1, "container.jpeg", GL_RGB);

    unsigned texture2;
    glGenTextures(1, &texture2);
    loadTexture(texture2, "awesomeface.png", GL_RGBA);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    CameraOption cameraOption;
    RenderOption renderOption;
    InputState inputState;

    GuiRenderContext guiCtx {
        .cameraOption = cameraOption,
        .renderOption = renderOption,
        .inputState = inputState,
    };
    // render loop
    // -----------
    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;

        // input
        // -----
        InputContext inputContext {
            .cameraOption = cameraOption,
            .deltaTime = deltaTime,
        };
        processInput(window, inputContext);
        cameraOption.pitch = pitch;
        cameraOption.yaw = yaw;
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float playbackValue = renderOption.manual ? renderOption.playback : currentTime;
        auto ratio = (sin(playbackValue) / 2.0f) + 0.5f;

        auto& co = cameraOption;
        glm::mat4 view, projection;
        view = glm::lookAt(co.pos, co.pos + co.front(), co.up);
        projection = glm::perspective(glm::radians(co.fovyDeg), co.aspectWidth / co.aspectHeight, co.zNear, co.zFar);

        // draw our first triangle
        ourShader.use();
        ourShader.setInt("texture1", 0);
        ourShader.setInt("texture2", 1);
        ourShader.setFloat("mixRatio", ratio);
        ourShader.setMatrix4f("view", view);
        ourShader.setMatrix4f("projection", projection);


        float greenValue = (sin(playbackValue) / 2.0f) + 0.5f;
        ourShader.set4f("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        for (unsigned i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            auto pos = cubePositions[i];
            model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
            model = glm::rotate(model, float(log(i) + 2) * playbackValue, glm::vec3(float(i) / 10.f, 1.0f, 0.0f));
            ourShader.setMatrix4f("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

        gui.render(guiCtx);

        glfwSwapBuffers(window);

        if (renderOption.continuous) {
            glfwPostEmptyEvent();
        }
        glfwWaitEvents();

        lastTime = currentTime;
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, InputContext &ctx)
{
    auto& co = ctx.cameraOption;
    float cameraDelta = ctx.cameraOption.speed * ctx.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        co.pos += co.front() * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        co.pos -= co.front() * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        co.pos -= glm::normalize(glm::cross(co.front(), co.up)) * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        co.pos += glm::normalize(glm::cross(co.front(), co.up)) * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        co.pos -= co.up * cameraDelta;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        co.pos += co.up * cameraDelta;
    static int prevSpace = GLFW_RELEASE;
    int currentSpace = glfwGetKey(window, GLFW_KEY_SPACE);
    if (prevSpace == GLFW_RELEASE && currentSpace == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cameraRotateMode = false;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cameraRotateMode = true;
        }
    }
    prevSpace = currentSpace;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void loadTexture(unsigned textureId, const char *filename, unsigned glFormat) {
    int width, height, nrChannels;
    std::string path = std::string("resources/") + filename;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.data(), &width, &height, &nrChannels, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void mouse_callback(GLFWwindow* window, double xPosD, double yPosD) {
    if (!cameraRotateMode) return;
    auto xPos = (float)xPosD, yPos = (float)yPosD;
    static float lastX = xPos, lastY = yPos;
    static float sensitivity = 0.1f;
    float xOffset = xPos - lastX, yOffset = yPos - lastY;

    yaw += xOffset * sensitivity;
    pitch -= yOffset * sensitivity;

    lastX = xPos;
    lastY = yPos;
}