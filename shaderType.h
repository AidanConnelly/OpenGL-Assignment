//
// Created by m on 02/10/2019.
//

#ifndef OPENGLSETUP_SHADERTYPE_H
#define OPENGLSETUP_SHADERTYPE_H

class ShaderType{
public:
    virtual char * GetAsString() const =0 ;
    virtual int GetAsInt() const = 0 ;
};

class VertexShaderType: public ShaderType{
    char* GetAsString() const override{
        return "VERTEX";
    }

    int GetAsInt() const override{
        return GL_VERTEX_SHADER;
    }
};

class FragmentShaderType:public ShaderType{
    char* GetAsString() const override{
        return "FRAGMENT";
    }

    int GetAsInt() const override{
        return GL_FRAGMENT_SHADER;
    }
};

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#endif //OPENGLSETUP_SHADERTYPE_H
