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
#include "../graphics/Vertex.h"
#include "../graphics/Mesh.h"
#include "xmlNode.h"
#include "xmlParsingStackMember.h"
#include "bufferParseResult.h"
#include "typedefs.h"
#include "stringToFloatFast.h"
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"

struct paramInfo{
    paramInfo(int stride, int idx);

    unsigned stride;
    unsigned idx;
};

struct textureCoordinateData {
    xmlNode *ar;
    paramInfo s;
    paramInfo t;
};

struct colourData {
    xmlNode *ar;
    paramInfo re;
    paramInfo gr;
    paramInfo bl;
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
		while ((buffer[index] >= '0' && buffer[index] <= '9')||buffer[index]=='-')
		{
			floatArray[floatIndex] = parseAFloat(&index, buffer);
			index++;
			floatIndex++;
		}
		this->transform = glm::transpose(glm::make_mat4(floatArray));
		this->hasTransform = true;
	}

	void processInstanceGeometryTags(xmlNodeStore instance_geometryTags);

	parseNodeTagsResult(xmlNode matrix, xmlNodeStore instance_geometryTags)
	{
		setMatrixFromNode(matrix);
		processInstanceGeometryTags(instance_geometryTags);
	}
	parseNodeTagsResult(xmlNodeStore instance_geometryTags)
	{
		processInstanceGeometryTags(instance_geometryTags);
	}

	bool hasTransform = false;
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
    static std::string removeLeadingHash(const std::string& toRemove);

private:
	static void parseNodeTagNames(xmlNodeStore &nodes);
	static void populateMeshDataWithCorrectColourAndTextures(std::string directory, xmlNodeStore nodes,
                                                             std::vector<MeshData> &toReturn,
                                                             std::vector<meshParseResult>::value_type &meshParseResult,
                                                             std::vector<parseNodeTagsResult> &nodesTagsResults);

	static xmlNodeVector filterByTagName(const xmlNodeVector& nodes, const std::string& tagName);

	static xmlNodeStore filterByTagName(xmlNodeStore nodes, const std::string& tagName);

	static void checkForQuotes(char thisChar, int& stackPos, parseStack& stack, xmlParsingStackMember& state);

	static bool anyByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value);

	static xmlNodeStore parseNodes(const std::vector<char>& buffer, std::vector<xmlNode*>& toClean);

	static xmlNodeVector mapXmlNodes(const xmlNodeVector& input, std::function<xmlNode(const xmlNode&)> toMap);

	static void alterXmlNodes(xmlNodeStore& input, std::function<void(xmlNode*)> toMap);

	static xmlNodeStore parseFloatArrays(std::vector<char> buffer, xmlNodeStore floatArrays);

	static xmlNodeStore parseIndexBuffer(const std::vector<char>& buffer, xmlNodeStore indexBuffers);

	static bufferParseResult parseLargeBuffers(xmlNodeStore nodesWithTagName);

	static std::vector<meshParseResult> parseMeshTags(std::vector<char> buffer, xmlNodeStore nodes,
	                                                  bufferParseResult* largeBuffers,
                                                      std::vector<textureCoordinateData*>& texCoordDataToClean,
                                                      std::vector<colourData*>& colDataToClear);

	static std::vector<parseNodeTagsResult> parseNodeTags(std::vector<char> buffer, xmlNodeStore nodes);
	static meshParseResult parseTriangleTag(bufferParseResult* largeBuffers, std::string id, xmlNode tag, xmlNode* triangleTagPtr, std::vector<textureCoordinateData*>& texCoordDataToClean, std::vector<colourData*>& colDataToClear);

	static xmlNode* getSoleByTag(const xmlNodeStore& toSearchIn, std::string toSearchFor);

	static xmlNode getSoleByAttrib(const xmlNodeStore& lookIn, std::string attrib, std::string value);

	static xmlNode * soleLargeFloatChild(const xmlNode& parent, bufferParseResult* bpr);

	static float getViaParam(std::string toGet, xmlNode* sourceTag, xmlNode* parsedFloatArray, unsigned index);



    static paramInfo prepareParam(std::string toGet, xmlNode *sourceTag, xmlNode *parsedFloatArray);

	static diffuseTextureOrColour getFileNameFromMaterialID(xmlNodeStore nodes, std::string materialId);
};

#endif //OPENGLSETUP_DAEPARSER_H
