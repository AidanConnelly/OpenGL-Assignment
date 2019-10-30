//
// Created by m on 14/10/2019.
//


#ifndef OPENGLSETUP_DAEPARSER_H
#define OPENGLSETUP_DAEPARSER_H

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

struct paramInfo{
    paramInfo(int stride, int idx);

    unsigned stride;
    unsigned idx;
};

struct parseNodeTagsResult
{
public:
	parseNodeTagsResult(xmlNode matrix, xmlNodeStore instance_geometryTags)
	{
		auto buffer = matrix.buffer;
		int index = matrix.startIndex;
		do
		{
			index++;
		}
		while ('>' != (*buffer)[index]);
		index++;
		int floatIndex = 0;
		float floatArray[16];
		while ((*buffer)[index] >= '0' && (*buffer)[index] <= '9')
		{
			floatArray[floatIndex] = parseAFloat(&index, buffer);
			index++;
			floatIndex++;
		}
		//    this->transform = glm::make_mat4(floatArray);

		auto beginning = instance_geometryTags.begin();
		auto ending = instance_geometryTags.end();
		auto forEach = [&](xmlNode* node) -> void
		{
			auto IDs = this->IDs;
			auto id = node->getAttribute("url");
			IDs.insert(id);
		};
		std::for_each(beginning, ending, forEach);
	}

	//    glm::mat4 transform;
	std::set<std::string> IDs;
};

struct vertexDef
{
	float x;
	float y;
	float z;

	float nX;
	float nY;
	float nZ;

	float u;
	float v;
};

struct triangle
{
	unsigned v1i;
	unsigned v2i;
	unsigned v3i;
};

struct meshParseResult
{
	std::vector<vertexDef> vertexes;
	std::vector<triangle> triangles;
	std::string meshID;
};

class daeParser
{
public:
	static xmlNodeVector parse(std::vector<char> buffer);

private:
	static void parseNodeTagNames(std::vector<char>& buffer, xmlNodeStore& nodes);

	static xmlNodeVector filterByTagName(const xmlNodeVector& nodes, const std::string& tagName);

	static xmlNodeStore filterByTagName(xmlNodeStore nodes, const std::string& tagName);

	static void checkForQuotes(char thisChar, int* stackPos, parseStack* stack, xmlParsingStackMember* state);

	static xmlNodeStore parseNodes(const std::vector<char>* buffer);

	static xmlNodeVector mapXmlNodes(const xmlNodeVector& input, std::function<xmlNode(const xmlNode&)> toMap);

	static void alterXmlNodes(xmlNodeStore& input, std::function<void(xmlNode*)> toMap);

	static xmlNodeStore parseFloatArrays(std::vector<char> buffer, xmlNodeStore floatArrays);

	static xmlNodeStore parseIndexBuffer(std::vector<char> buffer, xmlNodeStore indexBuffer);

	static bufferParseResult parseLargeBuffers(xmlNodeStore nodesWithTagName);

	static std::vector<meshParseResult> parseMeshTags(std::vector<char> buffer, xmlNodeVector nodes,
	                                                  bufferParseResult* largeBuffers);

	static std::vector<parseNodeTagsResult> parseNodeTags(std::vector<char> buffer, xmlNodeVector nodes);

	static xmlNode* getSoleByTag(const xmlNodeStore& toSearchIn, std::string toSearchFor);

	static xmlNode getSoleByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value);

	static xmlNode * soleLargeFloatChild(const xmlNode& parent, bufferParseResult* bpr);

	static float getViaParam(std::string toGet, xmlNode* sourceTag, xmlNode* parsedFloatArray, unsigned index);


	static std::string removeLeadingHash(const std::string& toRemove);

    static paramInfo prepareParam(std::string toGet, xmlNode *sourceTag, xmlNode *parsedFloatArray);
};

#endif //OPENGLSETUP_DAEPARSER_H
