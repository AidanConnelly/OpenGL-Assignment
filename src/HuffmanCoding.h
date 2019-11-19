#pragma once

#include <math.h>
#include <vector>
#include "bitBuffer.h"

static const float ENCODING_STD_DEV = 1.37;
static const float LN_2 = 0.69314718056;
static const float ADDATIVE_COEF = 1.77992395792;

int length(float toEncode)
{
	const float x2_multi_coef = 1.0f / (2.0f * ENCODING_STD_DEV * ENCODING_STD_DEV * LN_2);
	return ceil(x2_multi_coef * toEncode * toEncode + ADDATIVE_COEF);
}

float boundaryFromLength(int length)
{
	float numerator = length - ADDATIVE_COEF;
	float denominator = 1.0f / (2.0f * ENCODING_STD_DEV * ENCODING_STD_DEV * LN_2);;
	return sqrt(numerator / denominator);
}

void encode(float toEncode, float tolerance, std::vector<char>& toWriteTo, int& bitIndex)
{
	float inDistribtuion = toEncode * ENCODING_STD_DEV;
	bool sign = toEncode > 0;
	writeBitToCharVector(toWriteTo, bitIndex, sign);
	const int huffmanLength = length(inDistribtuion);
	int remainingLength = huffmanLength;
	remainingLength -= 1;
	while (remainingLength > 1)
	{
		remainingLength -= 1;
		writeBitToCharVector(toWriteTo, bitIndex, false);
	}
	writeBitToCharVector(toWriteTo, bitIndex, true);

	if (!sign)
	{
		inDistribtuion *= -1;
	}
	float lowerBound = 0;
	if (huffmanLength != 2)
	{
		lowerBound = boundaryFromLength(huffmanLength - 1);
	}
	float upperBound = boundaryFromLength(huffmanLength - 0);
	float positionInSection = (inDistribtuion - lowerBound) / (upperBound - lowerBound);

	float subdivisions = (upperBound - lowerBound) / tolerance;
	int precision = 0;
	do
	{
		precision += 1;
		subdivisions /= 2;
	}
	while (subdivisions > 1);
	int max = 1 << precision;
	int intToEncode = max * positionInSection;
	if (intToEncode == max)
	{
		intToEncode = max - 1;
	}

	writeIntToDigits(toWriteTo, bitIndex,intToEncode, precision);
}

float decode(std::vector<char>& toDecode, float tolerance, int& bitIndex)
{
	float sign = readBitFromCharVector(toDecode, bitIndex) ? 1.0 : -1.0;
	int huffmanLength = 2;
	while (!readBitFromCharVector(toDecode, bitIndex))
	{
		huffmanLength++;
	}
	float lowerBound = 0;
	if (huffmanLength != 2)
	{
		lowerBound = boundaryFromLength(huffmanLength - 1);
	}
	float upperBound = boundaryFromLength(huffmanLength - 0);
	float range = (upperBound - lowerBound);
	float subdivisions = range / tolerance;
	int precision = 0;
	do
	{
		precision += 1;
		subdivisions /= 2;
	}
	while (subdivisions > 1);
	int max = 1 << precision;
	int value = readIntToDigits(toDecode,bitIndex,precision);
	float asFloat = ((float)value) / ((float)max);
	float inDistribution = ((asFloat * range) + lowerBound);
	return sign * inDistribution / ENCODING_STD_DEV;
}

void encodeArray(std::vector<char>& toWriteTo, int& bitIndex, std::vector<float> toWrite, float tolerance)
{
	for (float& floatToWrite : toWrite)
	{
		encode(floatToWrite, tolerance, toWriteTo, bitIndex);
	}
}

std::vector<float> decodeArray(std::vector<char>& toDecode, int& bitIndex, int count, float tolerance)
{
	std::vector<float> toReturn;
	for (int i = 0; i < count; i++)
	{
		float toPush = decode(toDecode, tolerance, bitIndex);
		toReturn.push_back(toPush);
	}
	return toReturn;
}
