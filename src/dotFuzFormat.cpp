#ifndef dot_fuz_cpp
#define dot_fuz_cpp

#include <map>
#include <vector>
#include "bitBuffer.h"
#include "GMM.h"
#include "Mesh.h"

void writeFloat32(std::vector<char>& buffer, int& bitIndex, float toWrite){
    //Write bits
    int asInt = *((int*)(void*)&toWrite);
	for (int i = 32; i >= 0; i--)
	{
		bool bit = ((1 << i) & asInt);
		writeBitToCharVector(buffer, bitIndex, bit);
	}
}
void writeInt32(std::vector<char>& buffer, int& bitIndex, int toWrite){
    writeIntToDigits(buffer, bitIndex, toWrite, 32);   
}
void writeTolerance(std::vector<char>& buffer, int& bitIndex, float tolerance){
    writeFloat32(buffer, bitIndex, tolerance);
}
void writeNumberOfClusters(std::vector<char>& buffer, int& bitIndex, int clusters){
    writeInt32(buffer, bitIndex, clusters);
}
void writeNumberOfTriangles(std::vector<char>& buffer, int& bitIndex, int triangles){
    writeInt32(buffer, bitIndex, triangles);
}
void writeNumberOfVertexes(std::vector<char>& buffer, int& bitIndex, int vertexes){
    writeInt32(buffer, bitIndex, vertexes);
}
void writeClusterParameters(std::vector<char>& buffer, int& bitIndex, gmm toFit){
    for(int i = 0;i<toFit.means.size();i++){
        writeFloat32(buffer, bitIndex, toFit.means[i]);
        writeFloat32(buffer, bitIndex, toFit.width[i]);
    }
}

void writeFloatBufferIndexesAndLengths(std::vector<char>& buffer, int& bitIndex, int numberOfClusters, std::map<int,int> distributionFloatCounts, std::map<int,int> distributionStarts){
    for(int i = 0;i<numberOfClusters;i++){
        int count = 0;
        int start = 0;
        if(distributionStarts.count(i)==1){
            count = distributionFloatCounts[i];
            start = distributionStarts[i];
        }
        writeInt32(buffer, bitIndex, start);
        writeInt32(buffer, bitIndex, count);
    }

}
void writeTriangleVertexIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int> triangleIndexes, int digits){
    for(int i = 0;i<triangleIndexes.size();i++){
        writeIntToDigits(buffer, bitIndex, triangleIndexes[i],digits);
    }
}
void writeVertexFloatIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int> finalLookup, int digits){
    for(int i = 0;i<finalLookup.size();i++){
        writeIntToDigits(buffer, bitIndex, finalLookup[i],digits);
    }
}

float readFloat32(std::vector<char>& buffer, int& bitIndex){
    //Read bits
    int readed = 0x00;
	for (int i = 32; i >= 0; i--)
	{
        readed |= readBitFromCharVector(buffer,bitIndex)?1<<i:0;
	}
    float asFloat = *((float*)(void*)&readed);
    return asFloat;
}
int readInt32(std::vector<char>& buffer, int& bitIndex){
    return readIntToDigits(buffer, bitIndex, 32);   
}
float readTolerance(std::vector<char>& buffer, int& bitIndex){
    return readFloat32(buffer, bitIndex);
}
int readNumberOfClusters(std::vector<char>& buffer, int& bitIndex){
    return readInt32(buffer, bitIndex);
}
int readNumberOfTriangles(std::vector<char>& buffer, int& bitIndex){
    return readInt32(buffer, bitIndex);
}
int readNumberOfVertexes(std::vector<char>& buffer, int& bitIndex){
    return readInt32(buffer, bitIndex);
}

gmm readClusterParameters(std::vector<char>& buffer, int& bitIndex, int clusters){
    gmm toReturn;
    toReturn.init(clusters);
    for(int i = 0;i<clusters;i++){
        toReturn.means[i] = readFloat32(buffer, bitIndex);
        toReturn.width[i] = readFloat32(buffer, bitIndex);
    }
    return toReturn;
}

void readFloatBufferIndexesAndLengths(std::vector<char>& buffer, int& bitIndex, int numberOfClusters, std::map<int, int>& distributionStarts, std::map<int,int>& distributionFloatCounts){
    for(int i = 0;i<numberOfClusters;i++){
	    int start = readInt32(buffer, bitIndex);
        distributionStarts[i] = start;
	    int count = readInt32(buffer, bitIndex);
        distributionFloatCounts[i] = count;
    }
}

void readTriangleVertexIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int>& triangleIndexes, int digits, int triangleIndexs){
    for(int i = 0;i<triangleIndexs;i++){
        triangleIndexes.push_back(readIntToDigits(buffer, bitIndex, digits));
    }
}
void readVertexFloatIndexes(std::vector<char>& buffer, int& bitIndex, std::vector<int>& vertexIndexes, int digits, int vertexFloatIndexes){
    for(int i = 0;i<vertexFloatIndexes;i++){
        vertexIndexes.push_back(readIntToDigits(buffer, bitIndex, digits));
    }
}

#endif