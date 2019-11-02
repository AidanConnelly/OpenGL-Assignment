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

class MeshData{
    std::vector<Texture> textures;
public:
    MeshData(std::vector<Vertex> vertexes, std::vector<Triangle> triangles){
        this->vertexes = std::move(vertexes);
        this->triangles = std::move(triangles);
    }

    std::vector<Triangle> triangles;
    std::vector<Vertex> vertexes;
};

class Mesh {
public:
    Mesh(
            std::vector<Vertex>* vrtxes,
            std::vector<Triangle>* triags,
            std::vector<Texture>* textures
    ) {
        this->vertexes = *vrtxes;
        this->triangles = *triags;
        this->textures = *textures;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        unsigned int cnt = vertexes.size();
        auto *pos = (float*) &vertexes[0];
        float x0 = pos[0];
        float x1 = pos[1];
        size_t sizeOfVertex = sizeof(Vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeOfVertex * cnt, pos, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        unsigned int trigCount = triangles.size();
        size_t shouldb12Bytes = sizeof(Triangle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, shouldb12Bytes * trigCount, &triangles[0], GL_STATIC_DRAW);
        auto ErrorCheckValue = glGetError();
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
    explicit MeshInstance(Mesh* instanceOf) : instanceOf(instanceOf) {}

    void Draw() {
        glBindVertexArray(instanceOf->VAO);
        glDrawArrays(GL_TRIANGLES, instanceOf->VAO, 1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instanceOf->EBO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);



        auto ErrorCheckValue = glGetError();
        std::cout<<"";
    }

private:
    Mesh* instanceOf;
};

#endif //OPENGLSETUP_MESH_H
