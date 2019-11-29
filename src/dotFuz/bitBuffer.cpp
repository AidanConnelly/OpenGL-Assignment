#include <iostream>
#ifndef bit_buffer_cpp
#define bit_buffer_cpp

#include <math.h>
#include <vector>

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

void writeIntToDigits(std::vector<char>& buffer, int& bitIndex, int toWrite, int digits) {
	if(digits<32&&toWrite>(1<<digits))
	{
		std::cout << "wtf";
	}
	//Write bits
	for (int i = digits-1; i >= 0; i--)
	{
		bool bit = 1 << i & toWrite;
		writeBitToCharVector(buffer, bitIndex, bit);
	}
}

int readIntToDigits(std::vector<char>& buffer, int& bitIndex, int digits) {
	int value = 0;
	for (int i = digits-1; i >= 0; i--)
	{
		int bitReading = 1 << i;
		bool bitValue = readBitFromCharVector(buffer, bitIndex);
		value += bitValue ? bitReading : 0;
	}
	return value;
}

#endif