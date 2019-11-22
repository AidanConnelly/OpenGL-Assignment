#ifndef dot_fuz_h
#define dot_fuz_h

#include "Mesh.h"
#include <map>
#include <vector>
#include "GMM.h"

typedef unsigned int ContigousIndex;

struct listOfFloat
{
	ContigousIndex index;
	std::vector<float> floats;
};

void processVertex(float tolerance, std::map<int, listOfFloat*>& floatLookup, std::vector<int>& vertexFloatIndexes, Vertex vertex)
{
	for (int i = 0; i < sizeof(Vertex)/4; i ++)
	{
		void* vrtxPtr = (void*)(&vertex);
		float thisFloat = ((float*)vrtxPtr)[i];
		int approximation = thisFloat / tolerance;
		if (floatLookup.count(approximation) == 0)
		{
			const ContigousIndex size = floatLookup.size();
			floatLookup[approximation] = new listOfFloat{size, std::vector<float>{thisFloat}};
		}
		else
		{
			floatLookup[approximation]->floats.push_back(thisFloat);
		}
		vertexFloatIndexes.push_back(approximation);

	}
}

void processTriangle(std::vector<int>& triangleIndexes, Triangle triangle)
{
	for (int i = 0; i < sizeof(Triangle)/4; i ++)
	{
		void* trglPtr = (void*)(&triangle);
		int thisIndex = ((int*)trglPtr)[i];
		triangleIndexes.push_back(thisIndex);
	}
}

void writeFloatsOfMixture(std::vector<char>& buffer, int& bitIndex, float tolerance, std::vector<float> floatsToCluster, std::map<int, std::vector<int>> distributionToFloatsToClusterIndexMap, gmm toFit)
{
	int done = 0;
	for(int i= 0;i<toFit.means.size();i++){
		if(distributionToFloatsToClusterIndexMap.count(i)!=0){
			//Write floats
			for(int floatsToClusterIndex: distributionToFloatsToClusterIndexMap[i]){
				float inDistribution = toFit.inDistribution(floatsToCluster[floatsToClusterIndex],i);
				encode(inDistribution, tolerance, buffer, bitIndex);
				done++;
			}
		}
	}
	std::cout << done;
}

std::vector<float> readFloatsOfMixture(std::vector<char>& buffer, int&bitIndex, float tolerance, gmm distributions, std::map<int, int> distributionStarts, std::map<int, int>  distributionFloatCounts, int numberOfClusters){
	int done = 0;

	std::vector<float> toReturn;
	for (int i = 0; i < numberOfClusters; i++) {
		for(int j = 0;j< distributionFloatCounts[i];j++) {
			float inDistribution = decode(buffer, tolerance, bitIndex);
			float outOfDistribution = distributions.outDistribution(inDistribution, i) ;
			toReturn.push_back(outOfDistribution);
			done++;
		}
	}
	std::cout << done;

	return toReturn;
}

void writeInt32(std::vector<char>& buffer, int& bitIndex, int toWrite);
void writeTolerance(std::vector<char>& buffer, int& bitIndex, float tolerance);
void writeNumberOfClusters(std::vector<char>& buffer, int& bitIndex, int clusters);
void writeNumberOfTriangles(std::vector<char>& buffer, int& bitIndex, int triangles);
void writeNumberOfVertexes(std::vector<char>& buffer, int& bitIndex, int vertexes);
void writeClusterParameters(std::vector<char>& buffer, int& bitIndex, gmm toFit);
void writeFloatBufferIndexesAndLengths(std::vector<char>& buffer, int& bitIndex, int numberOfClusters, std::map<int,int> distributionFloatCounts, std::map<int,int> distributionStarts);
void writeTriangleVertexIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int> triangleIndexes, int digits);
void writeVertexFloatIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int> finalLookup, int digits);


float readFloat32(std::vector<char>& buffer, int& bitIndex);
int readInt32(std::vector<char>& buffer, int& bitIndex);
float readTolerance(std::vector<char>& buffer, int& bitIndex);
int readNumberOfClusters(std::vector<char>& buffer, int& bitIndex);
int readNumberOfTriangles(std::vector<char>& buffer, int& bitIndex);
int readNumberOfVertexes(std::vector<char>& buffer, int& bitIndex);
gmm readClusterParameters(std::vector<char>& buffer, int& bitIndex, int clusters);
void readFloatBufferIndexesAndLengths(std::vector<char>& buffer, int& bitIndex, int numberOfClusters, std::map<int,int>& distributionStarts, std::map<int, int>& distributionFloatCounts);
void readTriangleVertexIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int>& triangleIndexes, int digits, int triangleIndexs);
void readVertexFloatIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int>& vertexIndexes, int digits, int vertexFloatIndexes);

static std::vector<int> lastEncodeTriangleIndexes;
static std::vector<int> lastEncodeFloatIndexes;
static std::vector<float> lastFloatBuffer;

void encodeMultiMesh(MultiMesh* meshToEncode, float tolerance, std::vector<char>& buffer, int& bitIndex )
{
	std::vector<int> meshCounts;

	std::map<int, listOfFloat*> floatLookup;
	std::vector<int> vertexFloatIndexes;
	std::vector<int> triangleIndexes;

	//Also is the min
	int maxVertexIndex = 0x0;

	//As these shouldn't be negative
	//anyway, shouldn't be a problem
	int maxTriangleIndex = 0x0;

	for (Mesh* toEncode : meshToEncode->meshes)
	{
		meshCounts.push_back(toEncode->vertexes.size());
		meshCounts.push_back(toEncode->triangles.size());

		for (Vertex vertex : toEncode->vertexes)
		{
			processVertex(tolerance, floatLookup, vertexFloatIndexes, vertex);
		}
		for (Triangle triangle : toEncode->triangles)
		{
			processTriangle(triangleIndexes, triangle);
		}
	}
	
	std::vector<float> floatsToCluster;
	for (auto& pair : floatLookup)
	{
		floatsToCluster.push_back(0);
	}
	for (std::map<int, listOfFloat*>::value_type& pair : floatLookup)
	{
		float sum = 0;
		for (float f : pair.second->floats)
		{
			sum += f;
		}
		ContigousIndex index = pair.second->index;
		floatsToCluster[index] = sum / ((float)pair.second->floats.size());
	}

	gmm toFit;
	int clusters = 16;
	if (clusters > floatsToCluster.size() / 12)
	{
		clusters = floatsToCluster.size() / 12;
	}
	toFit.init(clusters);
	int iterations = 800;
	if(floatsToCluster.size() > 4000)
	{
		iterations = (500 * 4000)/floatsToCluster.size();
	}
	toFit.fit(floatsToCluster, iterations);
	std::cout << "clustered";

	std::vector<int> distribution;
	std::vector<int> inDistributionIndex;
	std::map<int,int> distributionFloatCounts;
	std::map<int,std::vector<int>> distributionToFloatsToClusterIndexMap;
	for(int i = 0;i<floatsToCluster.size();i++){
		int distrib = toFit.gaussian(floatsToCluster[i]);
		distribution.push_back(distrib);
		if(distributionFloatCounts.count(distrib)==0){
			distributionToFloatsToClusterIndexMap[distrib] = std::vector<int>{ i };
			distributionFloatCounts[distrib] = 1;
		}
		else{
			distributionToFloatsToClusterIndexMap[distrib].push_back(i);
			distributionFloatCounts[distrib]++;
		}
		inDistributionIndex.push_back(distributionFloatCounts[distrib]-1);
	}

	std::cout << std::endl << "clusters: " << distributionFloatCounts.size() << std::endl;
	
	int index = 0;
	int maxEnd = 0;
	std::map<int, int> distributionStarts;
	for(int i= 0;i<clusters;i++){
		if(distributionFloatCounts.count(i)!=0){
			distributionStarts[i] = index;
			index += distributionFloatCounts[i];
			int thisEnd = index;
			if(thisEnd>maxEnd)
			{
				maxEnd = thisEnd;
			}
		}
	}

	std::vector<int> finalLookup;
	for(int i = 0;i<vertexFloatIndexes.size();i++){
		int approxIndex = vertexFloatIndexes[i];
		listOfFloat* listOfFloat = floatLookup[approxIndex];
		int floatsToClusterIndex = listOfFloat->index;
		int inDistIdx = inDistributionIndex[floatsToClusterIndex];
		int distStartIdx = distributionStarts[distribution[floatsToClusterIndex]];
		int finalIdx = inDistIdx + distStartIdx;
		finalLookup.push_back( finalIdx);
	}
	
	lastEncodeTriangleIndexes = triangleIndexes;
	lastEncodeFloatIndexes = finalLookup;
	lastFloatBuffer = std::vector<float>();
	for (int i = 0; i < clusters; i++) {
		for (int floatsToClusterIndex : distributionToFloatsToClusterIndexMap[i]) {
			lastFloatBuffer.push_back(floatsToCluster[floatsToClusterIndex]);
		}
	}
	int numberOfTriangles = triangleIndexes.size() / 3;
	int numberOfVertexes = finalLookup.size() / 11;
	writeTolerance(buffer, bitIndex, tolerance);
	writeNumberOfClusters(buffer, bitIndex, clusters);
	writeNumberOfTriangles(buffer, bitIndex, numberOfTriangles);
	writeNumberOfVertexes(buffer, bitIndex, numberOfVertexes);
	writeClusterParameters(buffer, bitIndex, toFit);
	writeFloatBufferIndexesAndLengths(buffer, bitIndex, clusters, distributionFloatCounts, distributionStarts);
	writeFloatsOfMixture(buffer, bitIndex, tolerance, floatsToCluster, distributionToFloatsToClusterIndexMap, toFit);
	int triangleIndexesDigits = ceil(log2(numberOfVertexes));
	writeTriangleVertexIndexes(buffer, bitIndex, triangleIndexes,triangleIndexesDigits);
	int floatIndexesDigits = ceil(log2(maxEnd));
	writeVertexFloatIndexes(buffer, bitIndex, finalLookup, floatIndexesDigits);
}

MultiMesh* decodeMultiMesh(std::vector<char>& buffer){
	int bitIndex = 0;
	auto tolerance = readTolerance(buffer, bitIndex);
	auto clusters = readNumberOfClusters(buffer, bitIndex);
	auto triangles = readNumberOfTriangles(buffer, bitIndex);
	auto vertexes = readNumberOfVertexes(buffer, bitIndex);
	auto clusterParameters = readClusterParameters(buffer, bitIndex, clusters);
	std::map<int,int> distributionFloatCounts;
	std::map<int,int> distributionStarts;
	readFloatBufferIndexesAndLengths(buffer, bitIndex, clusters, distributionStarts, distributionFloatCounts);
	auto floatBuffer = readFloatsOfMixture(buffer, bitIndex, tolerance, clusterParameters, distributionStarts, distributionFloatCounts, clusters);;
	int triangleIndexesDigits = ceil(log2(vertexes));
	int maxEnd = 0;
	for(int i = 0;i<clusters;i++)
	{
		int thisEnd = distributionStarts[i] + distributionFloatCounts[i];
		if(thisEnd> maxEnd)
		{
			maxEnd = thisEnd;
		}
	}
	int floatIndexesDigits = ceil(log2(maxEnd));
	std::vector<int> triangleIndexes;
	std::vector<int> finalLookup;
	readTriangleVertexIndexes(buffer, bitIndex, triangleIndexes, triangleIndexesDigits, triangles * 3);
	readVertexFloatIndexes(buffer, bitIndex, finalLookup, floatIndexesDigits, vertexes * 11);

	bool problem = false;
	for (int i = 0; i < lastEncodeTriangleIndexes.size(); i++) {
		problem |= lastEncodeTriangleIndexes[i] != triangleIndexes[i];
	}
	for (int i = 0; i < lastEncodeFloatIndexes.size(); i++) {
		problem |= lastEncodeFloatIndexes[i] != finalLookup[i];
	}
	for (int i = 0; i < lastFloatBuffer.size(); i++) {
		problem |= abs(lastFloatBuffer[i] - floatBuffer[i])>2*tolerance;
	}

	std::cout << "read" << std::endl;
	std::vector<Vertex>* outputVertexes = new std::vector<Vertex>();
	std::vector<Triangle>* outputTriangles = new std::vector<Triangle>();
	for(int i = 0;i < finalLookup.size();i+=11)
	{
		Vertex v;
		void* vrtxPtr = (void*)(&v);
		float* vrtxFltPtr = (float*)vrtxPtr;
		for (int j = 0; j < 11; j++)
		{
			vrtxFltPtr[j] = floatBuffer[finalLookup[i + j]];
		}
		outputVertexes->push_back(v);
	}
	for(int i =0;i<triangleIndexes.size();i+=3)
	{
		Triangle t;
		t.v1i = triangleIndexes[i];
		t.v2i = triangleIndexes[i+1];
		t.v3i = triangleIndexes[i+2];
		outputTriangles->push_back(t);
	}
	Mesh* mesh = new Mesh(outputVertexes, outputTriangles, new std::vector<Texture>());
	std::vector<Mesh*> meshes = std::vector<Mesh*>{ mesh };
	MultiMesh* toReturn = new MultiMesh(meshes);
	return toReturn;
}

#endif