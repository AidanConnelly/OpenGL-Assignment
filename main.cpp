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
#include <experimental/filesystem>
#include <thread>

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

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -2.0f);
glm::mat4 cameraRotation = glm::mat4(1.0f);

std::map<std::string, int> cache;
std::vector<std::vector<MeshData>*> loaded;
int numMeshDataWrtn;
int numMeshDataRead;

struct makeInstancesJob
{
	int toMakeInstancesOf;
};

std::vector<makeInstancesJob> toMakeInstanceOf;
std::vector<MultiMesh*> meshes;
std::vector<MeshInstance> meshInstances;
int numMakeInstanceJobWrtn;
int numMakeInstanceJobRead;

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

	// std::string directory1 = "C:\\Users\\aidan\\..\\aidan\\Downloads\\Creeper\\";
	// std::string file = "relative.obj";
	// std::vector<char> objContents = fileThroughput::getBytes(directory1 + file);
	// std::vector<MeshData> results = objParser::parse(objContents, directory1);

	// std::string directory = R"(C:\Users\aidan\Downloads\Creeper-dae(1)\)";
	// std::vector<char> toParse = fileThroughput::getBytes(directory + "Creeper.dae");
	// std::vector<MeshData> results = daeParser::parse(toParse, directory);

	const ShaderType& vertexShaderType = VertexShaderType();

	Shader vShader = Shader("C:\\Users\\aidan\\Documents\\soft356a3\\shaders\\vertex.glsl", &vertexShaderType);

	const ShaderType& fragmentShaderType = FragmentShaderType();
	Shader fShader = Shader("C:\\Users\\aidan\\Documents\\soft356a3\\shaders\\fragment.glsl", &fragmentShaderType);

	ShaderProgram program = ShaderProgram();
	program.AttachShader(vShader);
	program.AttachShader(fShader);
	program.Link();

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
	auto start = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		for (int i = numMeshDataRead; i < numMeshDataWrtn; i++)
		{
			std::vector<Mesh*> toMakeMultiMeshOf;
			for (auto& a : *loaded[i])
			{
				a.BindTextures();
				Mesh* meshPtr = new Mesh(a);
				toMakeMultiMeshOf.push_back(meshPtr);
				numMeshDataRead = i + 1;
			}
			meshes.push_back(new MultiMesh(toMakeMultiMeshOf));
		}

		for (int i = numMakeInstanceJobRead; i < numMakeInstanceJobWrtn; i++)
		{
			int makeIndexOf = toMakeInstanceOf[i].toMakeInstancesOf;
			MultiMesh* mesh = meshes[makeIndexOf];
			MeshInstance instance = MeshInstance(mesh);
			meshInstances.push_back(instance);
			numMakeInstanceJobRead = i + 1;
		}

		processInput(window);

		glClearColor(0.09f, 0.12f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//        program.use();
		//        glBindVertexArray(VAO_Handle);
		program.use();

		int timeLoc = glGetUniformLocation(program.ID, "time");
		double time = (std::chrono::system_clock::now() - start).count() / 10000000.0;
		glUniform1f(timeLoc, time);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, cameraPosition);
		view = cameraRotation * view;

		// creating the projection matrix
		glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 50000.0f);
		for (int i = 0; i < meshInstances.size(); i++)
		{
			auto mI = meshInstances[i];
			mI.instanceOf->BindTextures(program);
			mI.selected = i == selected;
			mI.Draw(program, projection * view);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	//	std::string lmao;
	//	std::cin >> lmao;
	return 0;
}

void loopNavigation()
{
	auto current = std::experimental::filesystem::path("C://");
	while (true)
	{
		if (exists(current))
		{
			if (current.extension().empty())
			{
				try
				{
					auto currentDirContents = std::experimental::filesystem::directory_iterator(current);
					for (const auto& file : currentDirContents)
					{
						// std::string folderChar = "📁";
						std::string folderChar = "o";
						// std::string fileChar = "🗎";
						std::string fileChar = "i";
						std::cout << (std::experimental::filesystem::is_directory(file)
							              ? " " + folderChar + "\t"
							              : " " + fileChar + "\t");
						std::cout << file.path().filename() << std::endl;
					}
				}
				catch (const std::exception e)
				{
					std::cout << "error reading extensionless file system item as folder";
				}
			}
			else
			{
				std::cout << current.extension();
			}
		}

		std::string str;
		std::getline(std::cin, str);
		if (str == "exit")
		{
			return;
		}
		if (str.rfind("load ", 0) == 0)
		{
			std::string toLoad = str.substr(5, str.size() - 5);
			std::string fullFilePath = (current / toLoad).string();
			if (exists(current / toLoad))
			{
				std::vector<char> contents = fileThroughput::getBytes(fullFilePath);

				std::string extension = (current / toLoad).extension().string();

				std::vector<MeshData>* results = NULL;
				if (!cache.count(fullFilePath))
				{
					//todo
					if (extension == ".obj")
					{
						std::vector<MeshData> toResults = objParser::parse(contents, current.string() + "\\");
						results = new std::vector<MeshData>(std::move(toResults));
					}
					else if (extension == ".dae")
					{
						std::vector<MeshData> toResults = daeParser::parse(contents, current.string() + "\\");
						results = new std::vector<MeshData>(std::move(toResults));
					}
				}

				if (results)
				{
					loaded.push_back(results);
					cache[fullFilePath] = loaded.size() - 1;
				}
				int size = loaded.size();
				numMeshDataWrtn = size;

				//Add a mesh instance
				toMakeInstanceOf.push_back(makeInstancesJob{cache[fullFilePath]});
				int jobSize = toMakeInstanceOf.size();
				numMakeInstanceJobWrtn = jobSize;
			}
		}
		if (exists(current / str) && is_directory(current / str))
		{
			current = current / str;
		}
	}
}

int main(int argcp, char** argv)
{
	std::thread navigation(loopNavigation);
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
