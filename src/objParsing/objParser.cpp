#pragma once

#include "objParser.h"
#include "../safeIndex.h"
#include "../daeParsing/stringToFloatFast.h"
#include "../files/fileReader.h"

glm::vec3 parseColour(int&i,std::vector<char>&buffer){
	i += 2;
	float r, g, b;
	r = parseAFloat(&i, buffer);
	g = r;
	b = r;
	if (buffer[++i] != '\n')
	{
		g = parseAFloat(&i, buffer);
		if (buffer[++i] != '\n')
		{
			b = parseAFloat(&i, buffer);
		}
	}
	return glm::vec3(r,g,b);
}

MaterialLibParseResults* objParser::parseMtlLib(std::string fullPath, std::string directory)
{
	auto toReturn = new MaterialLibParseResults;
	std::vector<char> buffer = fileReader::read(fullPath);

	MTLParseState state = mtlNotBegun;
	LineReadingState lineState = beginningOfLine;
	std::string thisMtlName;
	for (int i = 0; i < buffer.size(); i++)
	{
		if (lineState == beginningOfLine)
		{
			if (buffer[i] == 'n')
			{
				while (buffer[i++] != ' ')
				{
				}
				thisMtlName = "";
				while (buffer[i] != '\n')
				{
					thisMtlName += buffer[i];
					i++;
				}
				toReturn->materials[thisMtlName] = mtlMaterial();
				state = mtlHasBegun;
			}
		}
		if (state == mtlHasBegun)
		{
			if (lineState == beginningOfLine)
			{
				lineState = middleOfLine;
				if (buffer[i] == 'K')
				{
					i++;
					if (buffer[i] == 'a')
					{
						//"Ka"
						toReturn->materials[thisMtlName].ambient = parseColour(i, buffer);
					}
					if (buffer[i] == 'd')
					{
						//"Kd"
						toReturn->materials[thisMtlName].diffuse = parseColour(i, buffer);
					}
					if (buffer[i] == 's')
					{
						//"Ks"
						toReturn->materials[thisMtlName].specular = parseColour(i, buffer);
					}
				}
				if (buffer[i] == 'm')
				{
					bool K = buffer[i + 4] == 'K';
					bool d = buffer[i + 5] == 'd';
					if (K && d)
					{
						i += 7;
						std::string filePath;
						while (buffer[i] != '\n')
						{
							filePath += buffer[i];
							i++;
						}
						std::cout << filePath << std::endl;
						std::string from_file = directory+filePath;
						toReturn->materials[thisMtlName].diffuseMap = from_file;
						toReturn->materials[thisMtlName].hasDiffuseMap = true;
					}
				}
				if(buffer[i]=='d'){
					i+=2;
					float d = parseAFloat(&i,buffer);
					toReturn->materials[thisMtlName].opacity = d;
				}
				if(buffer[i]=='T'){
					i+=3;
					float tr = parseAFloat(&i,buffer);
					toReturn->materials[thisMtlName].opacity = 1-tr;
				}
				if(buffer[i]=='N'){
				    i++;
				    if(buffer[i]=='s'){
				        i+=2;
				        float exponent = parseAFloat(&i,buffer);
				        toReturn->materials[thisMtlName].specularExponent = exponent;
				    }
				}
			}
		}

		if (lineState == middleOfLine)
		{
			if (i<buffer.size() && buffer[i] == '\n')
			{
				lineState = beginningOfLine;
			}
		}
	}
	return toReturn;
}

void objParser::nextLine(int& index, std::vector<char>& buffer)
{
	while (buffer[index] != '\n')
	{
		index++;
	};
}

glm::vec3 objParser::parse3SpaceSeperatedFloats(std::vector<char>& buffer, int* index)
{
	float x, y, z;
	(*index)++;
	x = parseAFloat(index, buffer);
	(*index)++;
	y = parseAFloat(index, buffer);
	(*index)++;
	z = parseAFloat(index, buffer);
	return glm::vec3(x, y, z);
}

glm::vec2 objParser::parse2SpaceSeperatedFloats(std::vector<char>& buffer, int* index)
{
	float x, y;
	(*index)++;
	x = parseAFloat(index, buffer);
	(*index)++;
	y = parseAFloat(index, buffer);
	return glm::vec2(x, y);
}

void objParser::assertVertexesArePlanarAndConvex(std::vector<Vertex> vertexes)
{
	//Check planar
	glm::vec3 v1to0 = vec3FromVertex(vertexes[0]) - vec3FromVertex(vertexes[1]);
	glm::vec3 v1to2 = vec3FromVertex(vertexes[2]) - vec3FromVertex(vertexes[1]);
	glm::vec3 normal = glm::normalize(glm::cross(v1to0, v1to2));

	int n = vertexes.size();
	for (int i = 0; i < vertexes.size(); i++)
	{
		glm::vec3 middleToPrev = vec3FromVertex(vertexes[(i + n - 1) % n]) - vec3FromVertex(vertexes[i % n]);
		glm::vec3 middleToNext = vec3FromVertex(vertexes[(i + 0 + 1) % n]) - vec3FromVertex(vertexes[i % n]);
		glm::vec3 thisNorm = glm::normalize(glm::cross(middleToPrev, middleToNext));

		const double THRESHOLD = 0.00001;
		float diff = glm::length(normal-thisNorm);
		if(diff>THRESHOLD)
		{
		}
	}
}

std::vector<MeshData> objParser::parse(std::vector<char>& buffer, std::string directory)
{
	std::map<std::string,MeshData> toReturn;
	std::vector<MaterialLibParseResults*> mtlLibParseResults;
	glm::vec3 currentColour = glm::vec3(0, 0, 0);
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> vertexCoordinates;
	std::string materialName;
	for (int i = 0; i < buffer.size(); i++)
	{
		switch (buffer[i])
		{
		case 'm':
			{
				while (buffer[i] != ' ')
				{
					i++;
				}
				i++;
				std::string fileName;
				while (buffer[i] != '\n')
				{
					fileName += buffer[i];
					i++;
				}
				mtlLibParseResults.push_back(parseMtlLib(directory + fileName,directory));
				nextLine(i, buffer);
				break;
			}
		case 'o':
			nextLine(i, buffer);
			//"object"
			break;
		case 'g':
			nextLine(i, buffer);
			//"group"
			break;
		case 's':
			nextLine(i, buffer);
			//smooth shading
			break;
		case'u': {
			while (buffer[i] != ' ')
			{
				i++;
			}
			i++;
			materialName = "";
			while (buffer[i] != '\n')
			{
				materialName += buffer[i];
				i++;
			}
			if(toReturn.count(materialName)==0){
				toReturn[materialName] = MeshData();
			}
			for (auto& mtlLibParseResult : mtlLibParseResults)
			{
				if(mtlLibParseResult->materials.count(materialName)==1){
					mtlMaterial m = mtlLibParseResult->materials[materialName];

					if(m.hasDiffuseMap){
						std::string toPush = m.diffuseMap;
						toReturn[materialName].texturePaths.push_back(toPush);
					}
					else{
						currentColour = m.diffuse;
					}

					toReturn[materialName].ambient = m.ambient;
					toReturn[materialName].specular = m.specular;
					toReturn[materialName].opacity = m.opacity;
					toReturn[materialName].specularExponent = m.specularExponent;
				}
			}
			nextLine(i, buffer);
			//"usemtl"
			break; }
		case'#':
			nextLine(i, buffer);
			break;
		case 'v':
			i++;
			switch (buffer[i])
			{
			case ' ':
				{
					glm::vec3 threeVecs = parse3SpaceSeperatedFloats(buffer, &i);
					nextLine(i, buffer);
					vertexPositions.push_back(threeVecs);
					//vertex
					break;
				}
			case'n':
				{
					i++;
					glm::vec3 threeVecs = parse3SpaceSeperatedFloats(buffer, &i);
					nextLine(i, buffer);
					vertexNormals.push_back(threeVecs);
					break;
				}
				//normal coordinates
			case't':
				{
					i++;
					glm::vec2 twoVecs = parse2SpaceSeperatedFloats(buffer, &i);
					nextLine(i, buffer);
					vertexCoordinates.push_back(twoVecs);
					//texture coordinates
					break;
				}
			default:
				throw std::invalid_argument("todo");
				break;
			}
			break;
		case'f':
			{
				i++;
				std::vector<Vertex> vertexes;
				while (buffer[i] != '\n')
				{
					i++;
					Vertex making;
					glm::vec3 vertexPosition;
					glm::vec3 vertexNormal;
					bool vertexNormalInitialized = false;
					glm::vec2 vertexCoordinate;
					bool vertexCoordinateInitialized = false;
					int vertexPositionIndex = parseAnInt(&i, buffer) - 1;
					vertexPosition = safeAt(vertexPositions,vertexPositionIndex);
					if (buffer[i] == '/')
					{
						i++;
						if (buffer[i] != '/')
						{
							//Was n/n/n or n/n
							int vertexCoordinateIndex = parseAnInt(&i, buffer) - 1;
							vertexCoordinate = safeAt(vertexCoordinates,vertexCoordinateIndex);
							vertexCoordinateInitialized = true;
						}
						else
						{
							//Was n//n or n/
						}
						if (buffer[i] == '/')
						{
							i++;
							//Was n//n or n/n/n
							int vertexNormalIndex = parseAnInt(&i, buffer) - 1;
							vertexNormal = safeAt(vertexNormals,vertexNormalIndex);
							vertexNormalInitialized = true;
						}
					}
					making.r = currentColour.x;
					making.g = currentColour.y;
					making.b = currentColour.z;
					making.x = vertexPosition.x;
					making.y = vertexPosition.y;
					making.z = vertexPosition.z;
					if (vertexNormalInitialized) {
						making.nX = vertexNormal.x;
						making.nY = vertexNormal.y;
						making.nZ = vertexNormal.z;
					}
					else {
						making.nX = 0;
						making.nY = 0;
						making.nZ = 0;
					}
					if (vertexCoordinateInitialized) {
						making.u = vertexCoordinate.x;
						making.v = vertexCoordinate.y;
					}
					else {
						making.u = 0;
						making.v = 0;
					}
					vertexes.push_back(making);
				}
				nextLine(i, buffer);
				assertVertexesArePlanarAndConvex(vertexes);

				unsigned offset = toReturn[materialName].vertexes.size();
				for(unsigned j = 0;j<vertexes.size();j++)
				{
					toReturn[materialName].vertexes.push_back(vertexes[j]);
					if(j>1)
					{
						toReturn[materialName].triangles.push_back(Triangle{ offset+0,offset+j - 1,offset+j });
					}
				}
				break;
			}
		default:
			throw std::invalid_argument("invalid state");
			break;
		}
	}
	std::vector<MeshData> actuallyReturn;
	for(auto &pair : toReturn){
		actuallyReturn.push_back(pair.second);
	}
	for(MaterialLibParseResults* cleanMe: mtlLibParseResults){
	    delete cleanMe;
	}
	return actuallyReturn;
}
