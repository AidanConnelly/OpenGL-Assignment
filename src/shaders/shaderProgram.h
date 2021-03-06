//
// Created by m on 02/10/2019.
//

#include "shader.h"
#ifndef OPENGLSETUP_SHADERPROGRAM_H
#define OPENGLSETUP_SHADERPROGRAM_H

class ShaderProgram{
public:
    ShaderProgram(){
        ID = glCreateProgram();
    }

    void AttachShader(Shader shader){
        glAttachShader(ID, shader.shader);
        glDeleteShader(shader.shader);
    }

    void Link(){
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
    }

    void use() {
        glUseProgram(ID);
    }
	unsigned ID;

private:

    static void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "shader error" << type << "\n" << infoLog
                          << "\n --- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "shader linking error" << type << "\n" << infoLog
                          << "\n --- " << std::endl;
            }
        }
    }
};


















#endif //OPENGLSETUP_SHADERPROGRAM_H
