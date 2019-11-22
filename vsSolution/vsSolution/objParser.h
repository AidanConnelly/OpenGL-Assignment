#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H


#include <string>
#include <vector>
#include "../../src/Mesh.h"
#include <map>
#include "../../experiments/preliminary/fileThroughput/fileThroughput.h"


struct mtlMaterial{
	float opacity;
	float specularExponent;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	std::string diffuseMap;

	bool hasDiffuseMap;
};

struct MaterialLibParseResults
{
	std::map<std::string, mtlMaterial> materials;
};


enum MTLParseState
{
	mtlNotBegun,
	mtlHasBegun,
};

enum LineReadingState
{
	beginningOfLine,
	middleOfLine,
};

class objParser
{
public:
	static MaterialLibParseResults* parseMtlLib(std::string fullPath, std::string directory);

	static void nextLine(int& index, std::vector<char>& buffer);
	static glm::vec3 parse3SpaceSeperatedFloats(std::vector<char>& buffer, int* index);
	static glm::vec2 parse2SpaceSeperatedFloats(std::vector<char>& buffer, int* index);
	static void assertVertexesArePlanarAndConvex(std::vector<Vertex> vertexes);

	static std::vector<MeshData> parse(std::vector<char>& buffer, std::string directory);

private:
};

#endif // !OBJ_PARSER_H
