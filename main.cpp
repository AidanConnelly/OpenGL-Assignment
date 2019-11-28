#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"

#include "shader.h"
#include "shaderProgram.h"
#include "experiments/preliminary/fileThroughput/fileThroughput.h"
#include "experiments/filesystem.h"
#include "src/Texture.h"
#include "src/Mesh.h"
#include <iostream>
#include "vsSolution/vsSolution/objParser.h"
#include <filesystem>
#include <thread>
#include "src/HuffmanCoding.h"
#include "src/GMM.h"
#include "src/dotFuzFormat.h"
#include "src/consoleControl.h"

std::vector<MultiMesh*> meshes;
std::vector<MeshInstance> meshInstances;
std::vector<Texture> overrideTextures;

ConsoleControl consoleControl;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 320;
const unsigned int SCR_HEIGHT = 320;

float vertices[] = {
	-0.61f, +0.61f, -1.00f,
	+0.61f, -0.61f, -1.00f,
	+0.00f, +0.61f, -2.00f,
	+0.00f, +0.61f, -1.00f,
};

unsigned int VAO_Handle;
unsigned int VBO_Handle;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, +4.5f);
glm::mat4 cameraRotation = glm::mat4(1.0f);

int selected = 0;

int openGLloop()
{
	////	for (xmlNode &r : results) {
	////		printf("%d |%s \t|\t%d\t-> %d\t", r.children.size(),r.tagName.c_str(), r.startIndex, r.endIndex);
	////		std::cout<< r.children.size()<<" |"<<r.tagName<<" \t|"<<r.startIndex<<"\t->\t"<<r.endIndex<<"\t ";
	////		for (int i = r.startIndex; i <= r.endIndex; i++) {
	////			std::cout << toParse[i];
	////		}
	////		std::cout << std::endl;
	////	}
	//
	// glfwInit();
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "soft356 part one", NULL, NULL);
	// if (window == NULL) {
	//     std::cout << "Failed to create GLFW window" << std::endl;
	//     glfwTerminate();
	//     return -1;
	// }
	//
	// glfwMakeContextCurrent(window);
	// glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//    glutInit(&argcp,argv);
	//
	//    glutCreateWindow("GLEW Test");
	// GLenum err = glewInit();
	// if (GLEW_OK != err)
	// {
	// 	/* Problem: glewInit failed, something is seriously wrong. */
	// 	fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	// }
	// fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "Textured Cube", NULL, NULL);

	glfwMakeContextCurrent(window);
	glewInit();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
//    glFrontFace(GL_CCW);

    // std::string directory1 = "C:\\Users\\aidan\\..\\aidan\\Downloads\\Creeper\\";
	// std::string file = "relative.obj";
	// std::vector<char> objContents = fileThroughput::getBytes(directory1 + file);
	// std::vector<MeshData> results = objParser::parse(objContents, directory1);

	// std::string directory = R"(C:\Users\aidan\Downloads\Creeper-dae(1)\)";
	// std::vector<char> toParse = fileThroughput::getBytes(directory + "Creeper.dae");
	// std::vector<MeshData> results = daeParser::parse(toParse, directory);

	const ShaderType& vertexShaderType = VertexShaderType();
    const ShaderType& geometryShaderType = GeometryShaderType();
    const ShaderType& fragmentShaderType = FragmentShaderType();
	std::string prefix = "..\\shaders\\";
    Shader meshVertexShader = Shader(prefix+"meshVertex.glsl", &vertexShaderType);
    Shader meshGeometryShader = Shader(prefix+"meshGeometry.glsl", &geometryShaderType);
	Shader meshFragmentShader = Shader(prefix + "meshFragment.glsl", &fragmentShaderType);
	ShaderProgram meshProgram = ShaderProgram();
	meshProgram.AttachShader(meshVertexShader);
	meshProgram.AttachShader(meshGeometryShader);
	meshProgram.AttachShader(meshFragmentShader);
	meshProgram.Link();

    Shader lightSourceVertexShader = Shader(prefix + "lightSourceVertex.glsl",&vertexShaderType);
    Shader lightSourceFragmentShader = Shader(prefix + "lightSourceFragment.glsl",&fragmentShaderType);
    ShaderProgram lightSourceProgram = ShaderProgram();
    lightSourceProgram.AttachShader(lightSourceVertexShader);
    lightSourceProgram.AttachShader(lightSourceFragmentShader);
    lightSourceProgram.Link();

    std::cout << "finished linking shader programs" << std::endl;
	//
	//    glGenVertexArrays(1, &VAO_Handle);
	//    glGenBuffers(1, &VBO_Handle);
	//
	//    glBindVertexArray(VAO_Handle);
	//
	//    glBindBuffer(GL_ARRAY_BUFFER, VBO_Handle);
	//
	//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
	//    glEnableVertexAttribArray(0);
	//
	//    glBindBuffer(GL_ARRAY_BUFFER, 0);


	std::vector<Texture> textures;


	//  -0.61f, +0.61f, -1.00f,
	//  +0.61f, -0.61f, -1.00f,
	//  +0.00f, +0.61f, -2.00f,

	// creating the view matrix
	glm::vec3 lightPos = glm::vec3(4.0, 4.0, 4.0);
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0, -1.0, -1.0));

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), glm::value_ptr(lightPos), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	auto start = std::chrono::system_clock::now();
	
	while (!glfwWindowShouldClose(window))
	{
		consoleControl.loadMeshesInto(meshes, meshInstances);
		consoleControl.loadOverrideTextures(overrideTextures);
		consoleControl.exportMesh(meshes, meshInstances);
		processInput(window);

		glClearColor(0.09f, 0.12f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double time = (std::chrono::system_clock::now() - start).count() / 1000000000.0;

        // creating the projection matrix
        glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 50000.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, -cameraPosition);
        view = cameraRotation * view;

        lightSourceProgram.use();
        int vLoc = glGetUniformLocation(lightSourceProgram.ID, "v");
        int pLoc = glGetUniformLocation(lightSourceProgram.ID, "p");
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_POINTS, 0, 1);

        auto ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR)
        {
            fprintf(
                    stderr,
                    "ERROR: Could not create a VBO: %s \n");
            std::cout << std::endl;
            exit(-1);
        }
        meshProgram.use();
		int timeLoc = glGetUniformLocation(meshProgram.ID, "time");
		glUniform1f(timeLoc, time);

        glUniform1f(glGetUniformLocation(meshProgram.ID, "ambientLight"), 0.2);
        glUniform3fv(glGetUniformLocation(meshProgram.ID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(meshProgram.ID, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform1f(glGetUniformLocation(meshProgram.ID, "lightPower"), 40);
        glUniform1f(glGetUniformLocation(meshProgram.ID, "specularSurfaceRoughness"),0.1);
        int cameraLocationUniformLocation = glGetUniformLocation(meshProgram.ID, "cameraLocation");
        glUniform3fv(cameraLocationUniformLocation, 1, glm::value_ptr(cameraPosition));


		for (int i = 0; i < meshInstances.size(); i++)
		{
			auto mI = meshInstances[i];
			if (overrideTextures.size() == 0)
			{
				mI.instanceOf->BindTextures(meshProgram);
			}
			else
			{
				int hasTextureLocation = glGetUniformLocation(meshProgram.ID, "hasTexture");
				glUniform1f(hasTextureLocation, 1.0f);
				overrideTextures[i % overrideTextures.size()].bind(meshProgram, 0);
			}
			mI.selected = i == selected;
            vLoc = glGetUniformLocation(meshProgram.ID, "v");
            pLoc = glGetUniformLocation(meshProgram.ID, "p");
            glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
            mI.Draw(meshProgram);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	//	std::string lmao;
	//	std::cin >> lmao;
	return 0;
}

int main(int argcp, char** argv)
{
	std::vector<char> buffer;
	//encode(1.46f,tolerance,buffer,index);
	//index = 0;
	//float f = decode(buffer, tolerance, index);
	// while (true) {
	int index = 0;
	// 	std::vector<char> buffer;
	// 	std::vector<float> floats = { -1.0f, -0.1f, -0.01f, -0.001f,-0.0001f, -0.0f, +0.0f, +0.0001f, +0.001f };
	// 	index = 0;
	// 	encodeArray(buffer, index, floats, tolerance);
	// 	index = 0;
	// 	std::vector<float> decoded = decodeArray(buffer, index, floats.size(), tolerance);
	// }
	//

	writeInt32(buffer, index, 5);
	index = 0;
	int read = readInt32(buffer, index);


	std::thread navigation([] { consoleControl.loopNavigation(); });
	return openGLloop();
}

void ifKeyMoveCamera(GLFWwindow* window, glm::vec3& camera, glm::vec3 toMove, int key)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		glm::vec3 newVal = camera + glm::inverse(glm::mat3(cameraRotation)) * toMove;
		camera = newVal;
	}
}

void ifKeyRotateCamera(GLFWwindow* window, glm::mat4& camera, glm::vec3 rotateAround, float rotate, int key)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		camera = glm::rotate(camera, rotate, glm::inverse(glm::mat3(cameraRotation)) * rotateAround);
	}
}

void ifKeyMoveMesh(GLFWwindow* window, glm::vec3 direction, int key)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
		glm::vec3 toMove = glm::normalize(direction);
		toMove *= 0.01;
		meshInstances[selected].move(toMove);
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		int n = meshInstances.size();
		selected = (selected + n + 1) % n;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		int n = meshInstances.size();
		selected = (selected + n - 1) % n;
	}

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		meshInstances[selected].scale(-0.01);
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		meshInstances[selected].scale(+0.01);
	}

	ifKeyMoveMesh(window, glm::vec3(-1.0, 0.0, 0.0), GLFW_KEY_F);
	ifKeyMoveMesh(window, glm::vec3(+1.0, 0.0, 0.0), GLFW_KEY_H);
	ifKeyMoveMesh(window, glm::vec3(0.0, +1.0, 0.0), GLFW_KEY_G);
	ifKeyMoveMesh(window, glm::vec3(0.0, -1.0, 0.0), GLFW_KEY_T);

	ifKeyRotateCamera(window, cameraRotation, glm::vec3(1.0, 0.0, 0.0), -0.01, GLFW_KEY_I);
	ifKeyRotateCamera(window, cameraRotation, glm::vec3(1.0, 0.0, 0.0), +0.01, GLFW_KEY_K);
	ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 1.0, 0.0), -0.01, GLFW_KEY_J);
	ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 1.0, 0.0), +0.01, GLFW_KEY_L);
	ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 0.0, 1.0), -0.01, GLFW_KEY_Q);
	ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 0.0, 1.0), +0.01, GLFW_KEY_E);

	ifKeyMoveCamera(window, cameraPosition, glm::vec3(0.0, 0.0, -0.01), GLFW_KEY_LEFT_CONTROL);
	ifKeyMoveCamera(window, cameraPosition, glm::vec3(0.0, 0.0, +0.01), GLFW_KEY_LEFT_SHIFT);
	ifKeyMoveCamera(window, cameraPosition, glm::vec3(0.0, -0.01, 0.0), GLFW_KEY_S);
	ifKeyMoveCamera(window, cameraPosition, glm::vec3(0.0, +0.01, 0.0), GLFW_KEY_W);
	ifKeyMoveCamera(window, cameraPosition, glm::vec3(-0.01, 0.0, 0.0), GLFW_KEY_A);
	ifKeyMoveCamera(window, cameraPosition, glm::vec3(+0.01, 0.0, 0.0), GLFW_KEY_D);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
