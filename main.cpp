#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"

#include "src/shaders/shader.h"
#include "src/shaders/shaderProgram.h"
#include "src/files/fileReader.h"
#include "src/graphics/Texture.h"
#include "src/graphics/Mesh.h"
#include <iostream>
#include "src/objParsing/objParser.h"
#include <filesystem>
#include <thread>
#include "src/dotFuz/HuffmanCoding.h"
#include "src/dotFuz/GMM.h"
#include "src/dotFuz/dotFuzFormat.h"
#include "src/consoleControl.h"

std::vector<MultiMesh *> meshes;
std::vector<MeshInstance> meshInstances;
std::vector<Texture> overrideTextures;

ConsoleControl consoleControl;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void drawMeshes(const ShaderProgram &meshProgram);

void shadowPassDrawMeshes(const ShaderProgram &shadowProgram);

void checkError();

unsigned int screenWidth = 800;
unsigned int scrHeight = 600;

glm::vec3 cameraPosition = glm::vec3(0.0f, +4.5f, 0.0f);
glm::mat4 zUpRotation = glm::rotate(glm::mat4(1.0), ((float) PI / 2), glm::vec3(1.0, 0, 0));
glm::mat4 cameraRotation = glm::rotate(zUpRotation, ((float) PI / 2), glm::vec3(0, 1.0, 0));

int selected = 0;
int selectedMicro = 0;

double lastTime = 0;
double thisTime = 0;
int openGLloop()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(800, 600, "Textured Cube", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    const ShaderType &vertexShaderType = VertexShaderType();
    const ShaderType &geometryShaderType = GeometryShaderType();
    const ShaderType &fragmentShaderType = FragmentShaderType();

    std::string prefix = "..\\shaders\\";

    Shader meshShadowVertexShader = Shader(prefix + "shadowMeshVertex.glsl", &vertexShaderType);
    Shader meshShadowGeometryShader = Shader(prefix + "shadowMeshGeometry.glsl", &geometryShaderType);
    Shader meshShadowFragmentShader = Shader(prefix + "shadowMeshFragment.glsl", &fragmentShaderType);
    ShaderProgram meshShadowProgram = ShaderProgram();
    meshShadowProgram.AttachShader(meshShadowVertexShader);
    meshShadowProgram.AttachShader(meshShadowGeometryShader);
    meshShadowProgram.AttachShader(meshShadowFragmentShader);
    meshShadowProgram.Link();

    Shader lightingMeshVertexShader = Shader(prefix + "lightingMeshVertex.glsl", &vertexShaderType);
    Shader lightingMeshFragmentShader = Shader(prefix + "lightingMeshFragment.glsl", &fragmentShaderType);
    ShaderProgram lightingMeshProgram = ShaderProgram();
    lightingMeshProgram.AttachShader(lightingMeshVertexShader);
    lightingMeshProgram.AttachShader(lightingMeshFragmentShader);
    lightingMeshProgram.Link();

    Shader lightSourceVertexShader = Shader(prefix + "lightSourceVertex.glsl", &vertexShaderType);
    Shader lightSourceFragmentShader = Shader(prefix + "lightSourceFragment.glsl", &fragmentShaderType);
    ShaderProgram lightSourceProgram = ShaderProgram();
    lightSourceProgram.AttachShader(lightSourceVertexShader);
    lightSourceProgram.AttachShader(lightSourceFragmentShader);
    lightSourceProgram.Link();

    Shader shadowVertexShader = Shader(prefix + "shadowPassVertex.glsl", &vertexShaderType);
    Shader shadowGeometryShader = Shader(prefix + "shadowPassGeometry.glsl", &geometryShaderType);
    Shader shadowFragmentShader = Shader(prefix + "shadowPassFragment.glsl", &fragmentShaderType);
    ShaderProgram shadowProgram = ShaderProgram();
    shadowProgram.AttachShader(shadowVertexShader);
    shadowProgram.AttachShader(shadowGeometryShader);
    shadowProgram.AttachShader(shadowFragmentShader);
    shadowProgram.Link();

    std::cout << "finished linking shader programs" << std::endl;

    std::vector<Texture> textures;

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glm::vec3 lightPos = glm::vec3(0,0,0);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), glm::value_ptr(lightPos), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    auto start = std::chrono::system_clock::now();

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    const unsigned int SHADOW_WIDTH = 3*256, SHADOW_HEIGHT = SHADOW_WIDTH;

    unsigned int startMap;
    unsigned int endMap;
    unsigned int centroidMap;
    unsigned int triangleMap;

    unsigned int *IDs[] = {&startMap, &endMap, &centroidMap};
    for (unsigned int *&ID: IDs) {
        glGenTextures(1, ID);
        glBindTexture(GL_TEXTURE_2D, *ID);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F,
//                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RG, GL_HALF_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16I,
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RG_INTEGER, GL_SHORT, NULL);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8I,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA_INTEGER, GL_BYTE, NULL);
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
        lightPos = glm::vec3(4.0, 4.0, 4.0);
        glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0, -1.0, -1.0+0.0*sin(thisTime)));

        float near_plane = 1.0f, far_plane = 1000.0f;
        glm::mat4 lightProjection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos,
                                          lightPos + lightDir,
                                          glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        consoleControl.loadMeshesInto(meshes, meshInstances);
        consoleControl.loadOverrideTextures(overrideTextures);
        consoleControl.exportMesh(meshes, meshInstances);
        consoleControl.removeMesh(meshes, meshInstances);
        processInput(window);


#ifdef _MSC_VER
        const double timeConversion = 1.0 / 10000000.0;
#endif
#ifdef __MINGW32__
        const double timeConversion = 1.0 / 1000000000.0;
#endif
        lastTime = thisTime;
        thisTime = (std::chrono::system_clock::now() - start).count() * timeConversion;

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        checkError();

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

        GLenum buffers[]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        glDrawBuffers(4, buffers);

        checkError();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkError();

        checkError();
        shadowProgram.use();
        checkError();

        glUniformMatrix4fv(glGetUniformLocation(shadowProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE,
                           glm::value_ptr(lightSpaceMatrix));

        checkError();

        shadowPassDrawMeshes(shadowProgram);
        checkError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkError();

        glViewport(0, 0, screenWidth, scrHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkError();

        glBindTexture(GL_TEXTURE_2D, depthMap);
        checkError();

        glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 50000.0f);

        // don't move camera - it's a spot/sun light
        glm::mat4 view = glm::mat4(1.0f);
        view = cameraRotation * view;

        checkError();

        glClearColor(0.06f, 0.08f, 0.10f, 1.0f);
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

        ShaderProgram meshProgram = consoleControl.shadows ? meshShadowProgram : lightingMeshProgram;

        meshProgram.use();

        if (consoleControl.shadows) {
            std::string uniformNames[] = {"startMap",
                                          "endMap",
                                          "centroidMap",
                                          "triangleMap",
                                          "depthMap"};
            unsigned int mapsIDs[] = {startMap,
                                      endMap,
                                      centroidMap,
                                      triangleMap,
                                      depthMap};
            for (int i = 0; i < 5; i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, mapsIDs[i]);
                GLint uniformLocation = glGetUniformLocation(meshProgram.ID, uniformNames[i].c_str());
                glUniform1i(uniformLocation, i);
            }


            checkError();

            glUniformMatrix4fv(glGetUniformLocation(meshProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE,
                               glm::value_ptr(lightSpaceMatrix));
        }

        int timeLoc = glGetUniformLocation(meshProgram.ID, "time");
        glUniform1f(timeLoc, thisTime);

        glUniform1f(glGetUniformLocation(meshProgram.ID, "shadowMapSize"), (float) SHADOW_WIDTH);
        glUniform1f(glGetUniformLocation(meshProgram.ID, "ambientLight"), 0.23);
        glUniform3fv(glGetUniformLocation(meshProgram.ID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(meshProgram.ID, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform1f(glGetUniformLocation(meshProgram.ID, "lightPower"), 40);
        glUniform1f(glGetUniformLocation(meshProgram.ID, "specularSurfaceRoughness"), 0.1);
        int cameraLocationUniformLocation = glGetUniformLocation(meshProgram.ID, "cameraLocation");
        glUniform3fv(cameraLocationUniformLocation, 1, glm::value_ptr(cameraPosition));


        // creating the projection matrix
        view = glm::mat4(1.0f);
        view = glm::translate(view, -cameraPosition);
        view = cameraRotation * view;

        vLoc = glGetUniformLocation(meshProgram.ID, "v");
        pLoc = glGetUniformLocation(meshProgram.ID, "p");
        glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
        drawMeshes(meshProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void checkError() {
    auto ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr, " error encountered \n");
        std::cout << std::endl;
    }
}

void drawMeshes(const ShaderProgram &meshProgram) {
    for (int i = 0; i < meshInstances.size(); i++) {
        auto mI = meshInstances[i];
        const int startingSlot = 5;
        if (overrideTextures.size() == 0) {
            mI.instanceOf->BindTextures(meshProgram, startingSlot);
        } else {
            int hasTextureLocation = glGetUniformLocation(meshProgram.ID, "hasTexture");
            glUniform1f(hasTextureLocation, 1.0f);
            overrideTextures[i % overrideTextures.size()].bind(meshProgram, startingSlot);
        }
        mI.selected = i == selected;
        mI.Draw(meshProgram);
    }
}

void shadowPassDrawMeshes(const ShaderProgram &shadowProgram) {
    for (int i = 0; i < meshInstances.size(); i++) {
        auto mI = meshInstances[i];
        mI.Draw(shadowProgram);
    }
}

int main(int argcp, char **argv) {
    std::thread navigation([] { consoleControl.loopNavigation(); });
    navigation.detach();
    return openGLloop();
}

void ifKeyMoveCamera(GLFWwindow *window, glm::vec3 &camera, glm::vec3 toMove, int key) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        glm::vec3 newVal = camera + glm::inverse(glm::mat3(cameraRotation)) * toMove;
        camera = newVal;
    }
}

void ifKeyRotateCamera(GLFWwindow *window, glm::mat4 &camera, glm::vec3 rotateAround, float rotate, int key) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        camera = glm::rotate(camera, rotate, glm::inverse(glm::mat3(cameraRotation)) * rotateAround);
    }
}

void ifKeyMoveMesh(GLFWwindow* window, glm::vec3 direction, int key)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
	{
        glm::vec3 toMove = direction;
        meshInstances[selected].move(toMove);
    }
}

void ifKeyRotateMesh(GLFWwindow* window, float amount, glm::vec3 around, int key)
{
    if (glfwGetKey(window, key) == GLFW_PRESS)
    {
        meshInstances[selected].rotate(around,amount);
    }
}

bool deleteKeyDown = false;

void processInput(GLFWwindow *window) {
    float secondsPassed = thisTime - lastTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, true);
    }

    const int microSelections = 30;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        int n = microSelections * meshInstances.size();
        selectedMicro = (selectedMicro + n + 1) % n;
        selected = selectedMicro / microSelections;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        int n = microSelections * meshInstances.size();
        selectedMicro = (selectedMicro + n - 1) % n;
        selected = selectedMicro / microSelections;
    }


    if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
        deleteKeyDown = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_RELEASE) {
        if (deleteKeyDown) {
            deleteKeyDown = false;
            meshInstances.erase(meshInstances.begin() + selected);
            selected = 0;
        }
    }

    float scaleSpeed = 0.51;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        meshInstances[selected].scale(scaleSpeed*secondsPassed*-1);
    }
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
        meshInstances[selected].scale(scaleSpeed*secondsPassed*+1);
    }

    float moveSpeed = 4.4;
    ifKeyMoveMesh(window, moveSpeed *secondsPassed*glm::vec3(-1.0, 0.0, 0.0), GLFW_KEY_F);
    ifKeyMoveMesh(window, moveSpeed *secondsPassed*glm::vec3(+1.0, 0.0, 0.0), GLFW_KEY_H);
    ifKeyMoveMesh(window, moveSpeed *secondsPassed*glm::vec3(0.0, +1.0, 0.0), GLFW_KEY_G);
    ifKeyMoveMesh(window, moveSpeed *secondsPassed*glm::vec3(0.0, -1.0, 0.0), GLFW_KEY_T);
    ifKeyMoveMesh(window, moveSpeed *secondsPassed*glm::vec3(0.0, 0.0,+1.0), GLFW_KEY_4);
    ifKeyMoveMesh(window, moveSpeed *secondsPassed*glm::vec3(0.0, 0.0, -1.0), GLFW_KEY_R);

    float lookSpeed = 57.0;
    ifKeyRotateCamera(window, cameraRotation, glm::vec3(1.0, 0.0, 0.0),lookSpeed*secondsPassed* -0.01, GLFW_KEY_I);
    ifKeyRotateCamera(window, cameraRotation, glm::vec3(1.0, 0.0, 0.0),lookSpeed*secondsPassed* +0.01, GLFW_KEY_K);
    ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 1.0, 0.0),lookSpeed*secondsPassed* -0.01, GLFW_KEY_J);
    ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 1.0, 0.0),lookSpeed*secondsPassed* +0.01, GLFW_KEY_L);
    ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 0.0, 1.0),lookSpeed*secondsPassed* -0.01, GLFW_KEY_Q);
    ifKeyRotateCamera(window, cameraRotation, glm::vec3(0.0, 0.0, 1.0),lookSpeed*secondsPassed* +0.01, GLFW_KEY_E);

    float cameraMoveSpeed = 4.4;
    ifKeyMoveCamera(window, cameraPosition,cameraMoveSpeed* secondsPassed*glm::vec3(0.0, 0.0, -1.0), GLFW_KEY_LEFT_CONTROL);
    ifKeyMoveCamera(window, cameraPosition,cameraMoveSpeed* secondsPassed*glm::vec3(0.0, 0.0, +1.0), GLFW_KEY_LEFT_SHIFT);
    ifKeyMoveCamera(window, cameraPosition, cameraMoveSpeed*secondsPassed*glm::vec3(0.0, -1.0, 0.0), GLFW_KEY_S);
    ifKeyMoveCamera(window, cameraPosition, cameraMoveSpeed*secondsPassed*glm::vec3(0.0, +1.0, 0.0), GLFW_KEY_W);
    ifKeyMoveCamera(window, cameraPosition, cameraMoveSpeed*secondsPassed*glm::vec3(-1.0, 0.0, 0.0), GLFW_KEY_A);
    ifKeyMoveCamera(window, cameraPosition, cameraMoveSpeed*secondsPassed*glm::vec3(+1.0, 0.0, 0.0), GLFW_KEY_D);

    float rotateSpeed = 2.1;
    ifKeyRotateMesh(window, rotateSpeed * secondsPassed,glm::vec3(0.0,0.0,+1.0), GLFW_KEY_Y);
    ifKeyRotateMesh(window, rotateSpeed * secondsPassed,glm::vec3(0.0,0.0,-1.0), GLFW_KEY_U);
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    screenWidth = width;
    scrHeight = height;
    glViewport(0, 0, width, height);
}
