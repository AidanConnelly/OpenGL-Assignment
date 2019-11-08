#pragma once
#include <string>
#include <vector>
#include "../../src/Mesh.h"

class objParser
{
public:
	static std::vector<MeshData> parse(std::vector<char>* buffer, std::string directory)
	{
		MeshData toReturn;
		for (int i = 0; i < buffer->size(); i++)
		{
			switch ((*buffer)[i])
			{
			case 'm':
				//"mtllib"
				break;
			case 'o':
				//"object"
				break;
			case 'g':
				//"group"
				break;
			case 's':
				//smooth shading
				break;
			case'u':
				//"usemtl"
				break;
			case 'v':
				i++;
				switch ((*buffer)[i])
				{
				case ' ':
					//vertex
					break;
				case't':
					i++;
					break;
					//texture coordinates
				case'n':
					i++;
					//normal coordinates
					break;
				default:
					throw std::invalid_argument("todo");
					break;
				}
				break;
			case'f':
				//todo
				break;
			default:
				throw std::invalid_argument("todo");
				break;
			}
		}
	}

private:
};
