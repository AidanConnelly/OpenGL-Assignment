#ifdef __MINGW32__
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#endif

#ifndef __MINGW32__
#include "GL/glew.h"
#include "GL/freeglut.h"
#endif

#include "shader.h"
#include "shaderProgram.h"
#include "experiments/preliminary/fileThroughput/fileThroughput.h"
#include "experiments/preliminary/prototypeParsers/prototypeDaeParser.h"
#include "experiments/preliminary/prototypeParsers/stringToFloatFast.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 32;
const unsigned int SCR_HEIGHT = 32;

float vertices[] = {
        -0.61f, +0.61f, -1.00f,
        +0.61f, -0.61f, -1.00f,
        +0.00f, +0.61f, -2.00f,
        +0.00f, +0.61f, -1.00f,
};

unsigned int VAO_Handle;
unsigned int VBO_Handle;

int main() {
    const char * flt = "-9900.001e-11 ";
    int index = 0;
    std::cout << std::endl;
    std::cout << std::endl;
	std::cout << "Max texture units: " << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;

	int s0 = 0;
	unsigned us0 = 0;
	int seven = 7;

	std::cout << (int)(s0 | seven);
	std::cout << "\n";
	std::cout << (int)(us0 | seven);
	std::cout << "\n";

	std::string stringToP = R"(

<a>
    <b>
        <c/>
        <c/>
        <c/>
    </b>
    <d>
        <f>
            <i/>
        </f>
	    <f>
        </f>
    </d>
    <b>
        <c/>
    </b>
</a>

<?x x="x" x="x"?>
<x x="x" x="x" x="x">
  <x>
    <x>
      <x>Blender User</x>
    </x>
  </x>
</x>

)";
	std::vector<char> data(stringToP.begin(), stringToP.end());

	std::vector<char> toParse = fileThroughput::getBytes();
	std::vector<xmlNode> results = prototypeDaeParser::parse(toParse);

	for (xmlNode &r : results) {
		printf("%d |%s \t|\t%d\t-> %d\t", r.children.size(),r.tagName.c_str(), r.startIndex, r.endIndex);
		std::cout<< r.children.size()<<" |"<<r.tagName<<" \t|"<<r.startIndex<<"\t->\t"<<r.endIndex<<"\t ";
		for (int i = r.startIndex; i <= r.endIndex; i++) {
			std::cout << toParse[i];
		}
		std::cout << std::endl;
	}

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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);



    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.09f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        glBindVertexArray(VAO_Handle);
        glDrawArrays(GL_TRIANGLES, 0, 4);

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