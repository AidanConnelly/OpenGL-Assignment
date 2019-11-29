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

void drawMeshes(const ShaderProgram &meshProgram);
void shadowPassDrawMeshes(const ShaderProgram &shadowProgram);

void checkError();

unsigned int screenWidth = 800;
unsigned int scrHeight = 600;

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
int selectedMicro = 0;

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

	// GLFWwindow *window = glfwCreateWindow(screenWidth, SCR_HEIGHT, "soft356 part one", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

    Shader shadowVertexShader = Shader(prefix + "shadowVertex.glsl",&vertexShaderType);
    Shader shadowGeometryShader = Shader(prefix + "shadowGeometry.glsl",&geometryShaderType);
    Shader shadowFragmentShader = Shader(prefix + "shadowFragment.glsl",&fragmentShaderType);
    ShaderProgram shadowProgram = ShaderProgram();
    shadowProgram.AttachShader(shadowVertexShader);
    shadowProgram.AttachShader(shadowGeometryShader);
    shadowProgram.AttachShader(shadowFragmentShader);
    shadowProgram.Link();

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

    float near_plane = 1.0f, far_plane = 1000.0f;
    glm::mat4 lightProjection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos,
                                      lightPos+lightDir,
                                      glm::vec3( 0.0f, 1.0f,  0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = SHADOW_WIDTH;

    unsigned int startMap;
    unsigned int endMap;
    unsigned int centroidMap;
    unsigned int triangleMap;

    unsigned int* IDs [] = {&startMap,&endMap, &centroidMap};
    for(unsigned int*& ID: IDs) {
        glGenTextures(1, ID);
        glBindTexture(GL_TEXTURE_2D, *ID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RG, GL_HALF_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (&ID - &IDs[0]), GL_TEXTURE_2D, *ID, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        checkError();
    }


    glGenTextures(1, &triangleMap);
    glBindTexture(GL_TEXTURE_2D, triangleMap);
    checkError();
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8I,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA_INTEGER,    GL_BYTE, NULL);
    checkError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    checkError();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 3, GL_TEXTURE_2D, triangleMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    checkError();


    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (!glfwWindowShouldClose(window))
	{
		consoleControl.loadMeshesInto(meshes, meshInstances);
		consoleControl.loadOverrideTextures(overrideTextures);
		consoleControl.exportMesh(meshes, meshInstances);
		processInput(window);


        double time = (std::chrono::system_clock::now() - start).count() / 1000000000.0;

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        checkError();

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

        GLenum buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
        glDrawBuffers( 4, buffers );

        checkError();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        checkError();

        checkError();
        shadowProgram.use();
        checkError();

        glUniformMatrix4fv(glGetUniformLocation(shadowProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        checkError();

        shadowPassDrawMeshes(shadowProgram);
        checkError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkError();

        // 2. then render scene as normal with shadow mapping (using depth map)
        glViewport(0, 0, screenWidth, scrHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkError();

        glBindTexture(GL_TEXTURE_2D, depthMap);
        checkError();

        // creating the projection matrix
        glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 50000.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, -cameraPosition);
        view = cameraRotation * view;

        checkError();

        glClearColor(0.09f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightSourceProgram.use();
        int vLoc = glGetUniformLocation(lightSourceProgram.ID, "v");
        int pLoc = glGetUniformLocation(lightSourceProgram.ID, "p");
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_POINTS, 0, 1);

        checkError();
        meshProgram.use();


        std::string uniformNames []= {"startMap",
                                      "endMap",
                                      "centroidMap",
                                      "triangleMap",
                                      "depthMap"};
        unsigned int mapsIDs []= {startMap,
                                      endMap,
                                      centroidMap,
                                      triangleMap,
                                      depthMap};
        for(int i= 0;i<5;i++){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mapsIDs[i]);
            GLint uniformLocation = glGetUniformLocation(meshProgram.ID, uniformNames[i].c_str());
            glUniform1i(uniformLocation, i);
        }

        checkError();

        glUniformMatrix4fv(glGetUniformLocation(meshProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        int timeLoc = glGetUniformLocation(meshProgram.ID, "time");
		glUniform1f(timeLoc, time);

        glUniform1f(glGetUniformLocation(meshProgram.ID, "shadowMapSize"), (float)SHADOW_WIDTH);
        glUniform1f(glGetUniformLocation(meshProgram.ID, "ambientLight"), 0.2);
        glUniform3fv(glGetUniformLocation(meshProgram.ID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(meshProgram.ID, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform1f(glGetUniformLocation(meshProgram.ID, "lightPower"), 40);
        glUniform1f(glGetUniformLocation(meshProgram.ID, "specularSurfaceRoughness"),0.1);
        int cameraLocationUniformLocation = glGetUniformLocation(meshProgram.ID, "cameraLocation");
        glUniform3fv(cameraLocationUniformLocation, 1, glm::value_ptr(cameraPosition));


        vLoc = glGetUniformLocation(meshProgram.ID, "v");
        pLoc = glGetUniformLocation(meshProgram.ID, "p");
        glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
        drawMeshes(meshProgram);

        glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	//	std::string lmao;
	//	std::cin >> lmao;
	return 0;
}

void checkError() {
    auto ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
                stderr,
                "ERROR: Could not create a VBO: %s \n");
        std::cout << std::endl;
        //exit(-1);
    }
}

void drawMeshes(const ShaderProgram &meshProgram) {
    for (int i = 0; i < meshInstances.size(); i++)
    {
        auto mI = meshInstances[i];
        const int startingSlot = 5;
        if (overrideTextures.size() == 0)
        {
            mI.instanceOf->BindTextures(meshProgram,startingSlot);
        }
        else
        {
            int hasTextureLocation = glGetUniformLocation(meshProgram.ID, "hasTexture");
            glUniform1f(hasTextureLocation, 1.0f);
            overrideTextures[i % overrideTextures.size()].bind(meshProgram, startingSlot);
        }
        mI.selected = i == selected;
        mI.Draw(meshProgram);
    }
}

void shadowPassDrawMeshes(const ShaderProgram &shadowProgram) {
    for (int i = 0; i < meshInstances.size(); i++)
    {
        auto mI = meshInstances[i];
        mI.Draw(shadowProgram);
    }
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

bool deleteKeyDown = false;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	const int microSelections = 30;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		int n = microSelections*meshInstances.size();
        selectedMicro = (selectedMicro + n + 1) % n;
        selected = selectedMicro/microSelections;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		int n = microSelections*meshInstances.size();
        selectedMicro = (selectedMicro + n - 1) % n;
        selected = selectedMicro/microSelections;
	}


	if(glfwGetKey(window,GLFW_KEY_DELETE)==GLFW_PRESS){
        deleteKeyDown = true;
	}
	if(glfwGetKey(window,GLFW_KEY_DELETE)==GLFW_RELEASE){
	    if(deleteKeyDown){
            deleteKeyDown = false;
            meshInstances.erase(meshInstances.begin()+selected);
            selected = 0;
        }
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
    screenWidth = width;
    scrHeight = height;
	glViewport(0, 0, width, height);
}
