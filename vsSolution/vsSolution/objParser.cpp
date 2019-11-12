﻿#pragma once

#include "objParser.h"

MaterialLibParseResults* objParser::parseMtlLib(std::string fullPath, std::string directory)
{
	auto toReturn = new MaterialLibParseResults;
	auto buffer = fileThroughput::getBytes(fullPath);

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
					if (buffer[++i] == 'd')
					{
						//"Kd"
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
						toReturn->diffuse.insert_or_assign(thisMtlName, glm::vec3(r, g, b));
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
						toReturn->diffuseMap.insert_or_assign(thisMtlName, from_file);
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
		const double THRESHOLD = 0.0000001;
		if(glm::length(normal-thisNorm)>THRESHOLD)
		{
			throw std::invalid_argument("not planar");
		}
	}
}

std::vector<MeshData> objParser::parse(std::vector<char>& buffer, std::string directory)
{
	std::vector<MeshData> toReturn;
	MeshData constructing;
	std::vector<MaterialLibParseResults*> mtlLibParseResults;
	// glm::vec3 currentColour;
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> vertexCoordinates;
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
			std::string materialName;
			while (buffer[i] != '\n')
			{
				materialName += buffer[i];
				i++;
			}
			for (auto& mtlLibParseResult : mtlLibParseResults)
			{
				if (mtlLibParseResult->diffuseMap.count(materialName) == 1)
				{
					std::string toPush = mtlLibParseResult->diffuseMap[materialName];
					constructing.texturePaths.push_back(toPush);
				}
			// 	// if (mtlLibParseResult->diffuse.count(materialName) == 1)
			// 	// {
			// 	// 	currentColour = mtlLibParseResult->diffuse[materialName];
			// 	// }
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
					glm::vec2 vertexCoordinate;
					int vertexPositionIndex = parseAnInt(&i, buffer) - 1;
					vertexPosition = vertexPositions[vertexPositionIndex];
					if (buffer[i] == '/')
					{
						i++;
						if (buffer[i] != '/')
						{
							//Was n/n/n or n/n
							int vertexCoordinateIndex = parseAnInt(&i, buffer) - 1;
							vertexCoordinate = vertexCoordinates[vertexCoordinateIndex];
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
							vertexNormal = vertexNormals[vertexNormalIndex];
						}
					}
					// making.r = currentColour.r;
					// making.g = currentColour.g;
					// making.b = currentColour.b;
					making.x = vertexPosition.x;
					making.y = vertexPosition.y;
					making.z = vertexPosition.z;
					making.nX = vertexNormal.x;
					making.nY = vertexNormal.y;
					making.nZ = vertexNormal.z;
					making.u = vertexCoordinate.s;
					making.v = vertexCoordinate.t;
					vertexes.push_back(making);
				}
				nextLine(i, buffer);
				// assertVertexesArePlanarAndConvex(vertexes);

				unsigned offset = constructing.vertexes.size();
				for(unsigned j = 0;j<vertexes.size();j++)
				{
					constructing.vertexes.push_back(vertexes[j]);
					if(j>1)
					{
						constructing.triangles.push_back(Triangle{ offset+0,offset+j - 1,offset+j });
					}
				}
				//todo
				break;
			}
		default:
			throw std::invalid_argument("todo");
			break;
		}
	}
	toReturn.push_back(constructing);
	return toReturn;
}
