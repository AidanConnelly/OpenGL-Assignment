#pragma once

#include "Mesh.h"
#include <map>
#include <vector>
#include "GMM.h"

struct listOfFloat
{
	int index;
	std::vector<float> floats;
};

void processVertex(float tolerance, std::map<int, listOfFloat>& floatLookup, std::vector<int>& vertexFloatIndexes,
                   int& maxVertexIndex, Vertex vertex)
{
	for (int i = 0; i < sizeof(Vertex)/4; i ++)
	{
		void* vrtxPtr = (void*)(&vertex);
		float thisFloat = ((float*)vrtxPtr)[i];
		int approximation = thisFloat / tolerance;
		if (floatLookup.count(approximation) == 1)
		{
			floatLookup[approximation].floats.push_back(thisFloat);
		}
		else
		{
			int size = vertexFloatIndexes.size();
			floatLookup[approximation] = listOfFloat{size, std::vector<float>{thisFloat}};
			vertexFloatIndexes.push_back(approximation);
			maxVertexIndex |= abs(approximation);
		}
	}
}

void processTriangle(std::vector<int>& triangleIndexes, int& maxTriangleIndex, Triangle triangle)
{
	for (int i = 0; i < sizeof(Triangle); i += 3)
	{
		void* trglPtr = (void*)(&triangle);
		int thisIndex = ((int*)trglPtr)[i];
		triangleIndexes.push_back(thisIndex);
		maxTriangleIndex |= thisIndex;
	}
}

void encodeMultiMesh(MultiMesh* meshToEncode, float tolerance)
{
	std::vector<int> meshCounts;

	std::map<int, listOfFloat> floatLookup;
	std::vector<int> vertexFloatIndexes;
	std::vector<int> triangleIndexes;

	//Also is the min
	int maxVertexIndex = 0x0;

	//As these shouldn't be negative
	//anyway, shouldn't be a problem
	int maxTriangleIndex = 0x0;

	int index = 0;
	for (Mesh* toEncode : meshToEncode->meshes)
	{
		meshCounts.push_back(toEncode->vertexes.size());
		meshCounts.push_back(toEncode->triangles.size());

		for (Vertex vertex : toEncode->vertexes)
		{
			processVertex(tolerance, floatLookup, vertexFloatIndexes, maxVertexIndex, vertex);
		}
		for (Triangle triangle : toEncode->triangles)
		{
			processTriangle(triangleIndexes, maxTriangleIndex, triangle);
		}
	}

	int bitsVertexIndex = 0;
	int bitsTriangleIndex = 0;
	while (maxVertexIndex)
	{
		maxVertexIndex /= 2;
		bitsVertexIndex++;
	}
	while (maxTriangleIndex)
	{
		maxTriangleIndex /= 2;
		bitsTriangleIndex++;
	}

	std::vector<float> floatsToCluster;
	for (auto& pair : floatLookup)
	{
		floatsToCluster.push_back(0);
	}
	for (std::pair<int, listOfFloat> pair : floatLookup)
	{
		float sum = 0;
		for (float f : pair.second.floats)
		{
			sum += f;
		}
		floatsToCluster[pair.second.index] = sum / pair.second.floats.size();
	}

	gmm toFit;
	int clusters = 16;
	if (2 * clusters > floatsToCluster.size())
	{
		clusters = floatsToCluster.size() / 12;
	}
	toFit.init(clusters);
	toFit.fit(floatsToCluster);
	std::cout << "clustered";
}
