#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "shaderProgram.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float vertices[] = {
        -0.61f, +0.61f, -1.00f,
        +0.61f, -0.61f, -1.00f,
        +0.00f, +0.61f, -2.00f,
        +0.00f, +0.61f, -1.00f,
};

unsigned int VAO_Handle;
unsigned int VBO_Handle;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "soft356 part one", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    const ShaderType &vertexShaderType = VertexShaderType();
    Shader vShader = Shader("shaders\\vertex.glsl", &vertexShaderType);

    const ShaderType &fragmentShaderType = FragmentShaderType();
    Shader fShader = Shader("shaders\\fragment.glsl", &fragmentShaderType);

    ShaderProgram program = ShaderProgram();
    program.AttachShader(vShader);
    program.AttachShader(fShader);
    program.Link();

    glGenVertexArrays(1, &VAO_Handle);
    glGenBuffers(1, &VBO_Handle);

    glBindVertexArray(VAO_Handle);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    std::cout << "Max texture units: " << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.09f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        glBindVertexArray(VAO_Handle);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}