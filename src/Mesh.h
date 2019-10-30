//
// Created by aidan on 30/10/2019.
//

#ifndef OPENGLSETUP_MESH_H
#define OPENGLSETUP_MESH_H


#include <vector>
#include "../shaderProgram.h"
#include "Texture.h"
#include "Vertex.h"

class Mesh {
public:
    void BindTextures() {
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0 + i);
        }
    }

    GLuint VAO;
    std::vector<Triangle> triangles;
private:
    std::vector<Vertex> vertexes;
    std::vector<Texture> textures;
    GLuint VBO;
    GLuint EBO;
};

class MeshInstance{
public:
    void Draw(ShaderProgram shader) {
        glBindVertexArray(instanceOf.VAO);
        glDrawElements(GL_TRIANGLES, instanceOf.triangles.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
private:
    Mesh instanceOf;
};
#endif //OPENGLSETUP_MESH_H
