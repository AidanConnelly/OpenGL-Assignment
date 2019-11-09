#pragma once
#include <string>
#include <vector>
#include "../../src/Mesh.h"
#include <map>
#include "../../experiments/preliminary/fileThroughput/fileThroughput.h"

struct MaterialLibParseResults
{
	std::map<std::string, glm::vec3> diffuse;
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
	static MaterialLibParseResults& parseMtlLib(std::string fullPath)
	{
		MaterialLibParseResults toReturn;
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
					while (buffer[i++] != '\n')
					{
						thisMtlName += buffer[i];
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
							r = parseAFloat(&i, &buffer);
							g = r;
							b = r;
							if (buffer[++i] != '\n')
							{
								g = parseAFloat(&i, &buffer);
								if (buffer[++i] != '\n')
								{
									b = parseAFloat(&i, &buffer);
								}
							}
							toReturn.diffuse.insert_or_assign(thisMtlName, glm::vec3(r, g, b));
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
							while (buffer[i++] != '\n')
							{
								filePath += buffer[i];
							}
							std::cout << filePath << std::endl;
						}
					}
				}
			}

			if (lineState == middleOfLine)
			{
				if (buffer[i] == '\n')
				{
					lineState = beginningOfLine;
				}
			}
		}
		return toReturn;
	}

	static void nextLine(int& index, std::vector<char>* buffer)
	{
		while ((*buffer)[++index] != '\n') {};
	}
	
	static std::vector<MeshData> parse(std::vector<char>* buffer, std::string directory)
	{
		std::vector<MeshData> toReturn;
		std::vector<MaterialLibParseResults> mtlLibParseResults;
		for (int i = 0; i < buffer->size(); i++)
		{
			switch ((*buffer)[i])
			{
			case 'm':
				{
					while ((*buffer)[i] != ' ')
					{
						i++;
					}
					i++;
					std::string fileName;
					while ((*buffer)[i] != '\n')
					{
						fileName += (*buffer)[i];
						i++;
					}
					mtlLibParseResults.push_back(parseMtlLib(directory + fileName));
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
			case'u':
				nextLine(i, buffer);
				//"usemtl"
				break;
			case'#':
				nextLine(i, buffer);
				break;
			case 'v':
				i++;
				switch ((*buffer)[i])
				{
				case ' ':
					nextLine(i, buffer);
					//vertex
					break;
				case't':
					i++;
					nextLine(i, buffer);
					break;
					//texture coordinates
				case'n':
					i++;
					nextLine(i, buffer);
					//normal coordinates
					break;
				default:
					throw std::invalid_argument("todo");
					break;
				}
				break;
			case'f':
				nextLine(i, buffer);
				//todo
				break;
			default:
				throw std::invalid_argument("todo");
				break;
			}
		}
		return toReturn;
	}

private:
};
