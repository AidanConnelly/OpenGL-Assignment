//
// Created by aidan on 30/10/2019.
//

#ifndef OPENGLSETUP_MESH_H
#define OPENGLSETUP_MESH_H


#include <utility>
#include <vector>
#include "../shaderProgram.h"
#include "Texture.h"
#include "Vertex.h"

class Mesh {
public:
    Mesh(
            std::vector<Vertex> vrtxes,
            std::vector<Triangle> triags,
            std::vector<Texture> textures
    ) {
        this->vertexes = vrtxes;
        this->triangles = triags;
        this->textures = textures;

        glGenVertexArrays(1, &VAO);
        auto ErrorCheckValue = glGetError();
        glBindVertexArray(VAO);
        ErrorCheckValue = glGetError();
        glGenBuffers(1, &VBO);
        ErrorCheckValue = glGetError();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        ErrorCheckValue = glGetError();

        float v[9] = {
                +0.61f, +0.61f, -1.00f,
                +0.61f, -0.61f, -1.00f,
                -0.61f, +0.61f, -1.00f};
        glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
        ErrorCheckValue = glGetError();
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);
        ErrorCheckValue = glGetError();
        glEnableVertexAttribArray(0);
        ErrorCheckValue = glGetError();

        glGenBuffers(1, &EBO);
        ErrorCheckValue = glGetError();
        unsigned int a[3] = {0, 1, 2};
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        ErrorCheckValue = glGetError();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3, a, GL_STATIC_DRAW);
        ErrorCheckValue = glGetError();
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR)
        {
            fprintf(
                    stderr,
                    "ERROR: Could not create a VBO: %s \n");

            exit(-1);
        }
    }

    void BindTextures() {
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0 + i);
        }
    }

    GLuint EBO;
    GLuint VAO;
    std::vector<Triangle> triangles;
private:
    std::vector<Vertex> vertexes;
    std::vector<Texture> textures;
    GLuint VBO;
};

class MeshInstance {
public:
    explicit MeshInstance(Mesh instanceOf) : instanceOf(std::move(instanceOf)) {}

    void Draw() {
        glBindVertexArray(instanceOf.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instanceOf.EBO);
//        unsigned int a[3] = {0, 1, 2};
        auto ErrorCheckValue = glGetError();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        ErrorCheckValue = glGetError();
        std::cout<<"";
//        glBindVertexArray(0);
    }

private:
    Mesh instanceOf;
};

#endif //OPENGLSETUP_MESH_H
