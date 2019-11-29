#ifndef SHADER_H
#define SHADER_H

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shaderType.h"

class Shader {
public:
    unsigned int shader;

    Shader(std::string path, const ShaderType *shaderType) {

        //debug statement
        std::cout << path;


        std::string fromFile;
        std::ifstream shaderFile;

        // ensure ifstream objects can throw exceptions:
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            shaderFile.open(path.c_str());
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            fromFile = shaderStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "Shader error " << std::endl;
        }

        const char *shaderCode = fromFile.c_str();
        int success;
        char infoLog[512];
        int asInt = shaderType->GetAsInt();
        shader = glCreateShader(asInt);
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader);
    }


private:

};

#endif
