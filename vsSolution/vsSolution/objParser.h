#pragma once
#include <string>
#include <vector>
#include "../../src/Mesh.h"
#include <map>
#include "../../experiments/preliminary/fileThroughput/fileThroughput.h"

struct MaterialLibParseResults
{
	std::map<std::string, glm::vec3> diffuse;
	std::map<std::string, std::string> diffuseMap;
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

	static void nextLine(int& index, std::vector<char>* buffer);
	static glm::vec3 parse3SpaceSeperatedFloats(std::vector<char>* buffer, int* index);
	static glm::vec2 parse2SpaceSeperatedFloats(std::vector<char>* buffer, int* index);
	static void assertVertexesArePlanarAndConvex(std::vector<Vertex> vertexes);

	static std::vector<MeshData> parse(std::vector<char>* buffer, std::string directory);

private:
};
