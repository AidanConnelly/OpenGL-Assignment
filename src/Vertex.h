//
// Created by aidan on 30/10/2019.
//

#ifndef OPENGLSETUP_VERTEX_H
#define OPENGLSETUP_VERTEX_H

#include "glm/vec3.hpp"

struct Vertex
{
	float x;
	float y;
	float z;

	float nX;
	float nY;
	float nZ;

    float r;
    float g;
    float b;

	float u;
	float v;
};

glm::vec3 vec3FromVertex(Vertex toVec);

struct Triangle
{
	unsigned int v1i;
	unsigned int v2i;
	unsigned int v3i;
};
#endif //OPENGLSETUP_VERTEX_H
