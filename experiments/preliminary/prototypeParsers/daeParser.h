//
// Created by m on 14/10/2019.
//


#ifndef OPENGLSETUP_DAEPARSER_H
#define OPENGLSETUP_DAEPARSER_H

#include <algorithm>
#include <string>
#include <vector>
#include <functional>
#include <set>
#include "xmlNode.h"
#include "xmlParsingStackMember.h"
#include "bufferParseResult.h"
#include "typedefs.h"
#include "stringToFloatFast.h"
#include "../../../src/Vertex.h"
#include "../../../src/Mesh.h"

struct paramInfo{
    paramInfo(int stride, int idx);

    unsigned stride;
    unsigned idx;
};

struct diffuseTextureOrColour
{
	enum which { tex, col, none };

	which which;
	std::string texture;
	glm::vec3 colour;
};

struct parseNodeTagsResult
{
public:
	void setMatrixFromNode(xmlNode matrix)
	{
		const std::vector<char>& buffer = matrix.buffer;
		int index = matrix.startIndex;
		do
		{
			index++;
		}
		while ('>' != buffer[index]);
		index++;
		int floatIndex = 0;
		float floatArray[16];
		while (buffer[index] >= '0' && buffer[index] <= '9')
		{
			floatArray[floatIndex] = parseAFloat(&index, buffer);
			index++;
			floatIndex++;
		}
		this->transform = glm::make_mat4(floatArray);
	}

	void processInstanceGeometryTags(xmlNodeStore instance_geometryTags)
	{
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

	parseNodeTagsResult(xmlNode matrix, xmlNodeStore instance_geometryTags)
	{
		setMatrixFromNode(matrix);
		processInstanceGeometryTags(instance_geometryTags);
	}
	parseNodeTagsResult(xmlNodeStore instance_geometryTags)
	{
		processInstanceGeometryTags(instance_geometryTags);
	}

	glm::mat4 transform;
	std::set<std::string> IDs;
};

struct meshParseResult
{
	bool hasColourFromVertex;
	std::vector<Vertex> vertexes;
	std::vector<Triangle> triangles;
	std::vector<std::string> textureIds;
	std::string meshID;
};

class daeParser
{
public:
	static std::vector<MeshData> parse(std::vector<char> &buffer, std::string directory);

private:
	static void parseNodeTagNames(std::vector<char>& buffer, xmlNodeStore& nodes);

	static xmlNodeVector filterByTagName(const xmlNodeVector& nodes, const std::string& tagName);

	static xmlNodeStore filterByTagName(xmlNodeStore nodes, const std::string& tagName);

	static void checkForQuotes(char thisChar, int* stackPos, parseStack* stack, xmlParsingStackMember* state);

	static bool anyByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value);

	static xmlNodeStore parseNodes(const std::vector<char>& buffer);

	static xmlNodeVector mapXmlNodes(const xmlNodeVector& input, std::function<xmlNode(const xmlNode&)> toMap);

	static void alterXmlNodes(xmlNodeStore& input, std::function<void(xmlNode*)> toMap);

	static xmlNodeStore parseFloatArrays(std::vector<char> buffer, xmlNodeStore floatArrays);

	static xmlNodeStore parseIndexBuffer(const std::vector<char>& buffer, xmlNodeStore indexBuffer);

	static bufferParseResult parseLargeBuffers(xmlNodeStore nodesWithTagName);

	static std::vector<meshParseResult> parseMeshTags(std::vector<char> buffer, xmlNodeVector nodes,
	                                                  bufferParseResult* largeBuffers);

	static std::vector<parseNodeTagsResult> parseNodeTags(std::vector<char> buffer, xmlNodeVector nodes);
	static meshParseResult parseTriangleTag(bufferParseResult* largeBuffers, std::string id, xmlNode tag, xmlNode* triangleTagPtr);

	static xmlNode* getSoleByTag(const xmlNodeStore& toSearchIn, std::string toSearchFor);

	static xmlNode getSoleByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value);

	static xmlNode * soleLargeFloatChild(const xmlNode& parent, bufferParseResult* bpr);

	static float getViaParam(std::string toGet, xmlNode* sourceTag, xmlNode* parsedFloatArray, unsigned index);


	static std::string removeLeadingHash(const std::string& toRemove);

    static paramInfo prepareParam(std::string toGet, xmlNode *sourceTag, xmlNode *parsedFloatArray);

	static diffuseTextureOrColour getFileNameFromMaterialID(xmlNodeStore nodes, std::string materialId);
};

#endif //OPENGLSETUP_DAEPARSER_H
