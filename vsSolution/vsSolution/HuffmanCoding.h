#pragma once

#include <math.h>
#include <vector>

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

void writeBitToCharVector(std::vector<char>& toWriteTo, int& bitIndex, bool bitToWrite)
{
	int charIndex = bitIndex / 8;
	int subIndex = bitIndex - 8 * charIndex;
	if (subIndex == 0)
	{
		char zeros = 0x00;
		toWriteTo.push_back(zeros);
	}
	int one_at_end = 128 * (bitToWrite ? 1 : 0);
	int our_one = one_at_end >> subIndex;

	// ReSharper disable once CppCStyleCast
	char result = toWriteTo[charIndex] | our_one;
	toWriteTo[charIndex] = result;
	bitIndex++;
}

bool readBitFromCharVector(std::vector<char>& toReadFrom, int& bitIndex)
{
	int charIndex = bitIndex / 8;
	int subIndex = bitIndex - 8 * charIndex;
	char atIndex = toReadFrom[charIndex];
	char masked = atIndex & 128 >> subIndex;
	bitIndex++;
	return masked != 0x00;
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

	//Write bits
	for (int i = (precision - 1); i >= 0; i--)
	{
		bool bit = 1 << i & intToEncode;
		writeBitToCharVector(toWriteTo, bitIndex, bit);
	}
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
	int value = 0;
	for (int i = (precision - 1); i >= 0; i--)
	{
		int bitReading = 1 << i;
		bool bitValue = readBitFromCharVector(toDecode, bitIndex);
		value += bitValue * bitReading;
	}
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
