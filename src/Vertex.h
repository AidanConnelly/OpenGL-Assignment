//
// Created by aidan on 30/10/2019.
//

#ifndef OPENGLSETUP_VERTEX_H
#define OPENGLSETUP_VERTEX_H

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <exception>
#include <stdexcept>
#include <functional>
#include <iterator>
#include <set>
#include "../experiments/preliminary/prototypeParsers/xmlNode.h"
#include "../experiments/preliminary/prototypeParsers/xmlParsingStackMember.h"
#include "../experiments/preliminary/prototypeParsers/bufferParseResult.h"
#include "../experiments/preliminary/prototypeParsers/typedefs.h"
#include "../experiments/preliminary/prototypeParsers/stringToFloatFast.h"
#include "../../../src/Vertex.h"

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

struct Triangle
{
	unsigned int v1i;
	unsigned int v2i;
	unsigned int v3i;
};
#endif //OPENGLSETUP_VERTEX_H
