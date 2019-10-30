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
#include "xmlNode.h"
#include "xmlParsingStackMember.h"
#include "bufferParseResult.h"
#include "typedefs.h"
#include "stringToFloatFast.h"
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
	unsigned v1i;
	unsigned v2i;
	unsigned v3i;
};
#endif //OPENGLSETUP_VERTEX_H
