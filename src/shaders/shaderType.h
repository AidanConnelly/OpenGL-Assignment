//
// Created by m on 02/10/2019.
//

#ifndef OPENGLSETUP_SHADERTYPE_H
#define OPENGLSETUP_SHADERTYPE_H

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"

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

class GeometryShaderType:public ShaderType{
    char* GetAsString() const override {
        return "GEOMETRY";
    }

    int GetAsInt() const override{
        return GL_GEOMETRY_SHADER;
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
#endif //OPENGLSETUP_SHADERTYPE_H
