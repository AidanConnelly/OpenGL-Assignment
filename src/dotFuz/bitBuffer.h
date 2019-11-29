#ifndef bit_buffer_h
#define bit_buffer_h

#include <math.h>
#include <vector>

void writeBitToCharVector(std::vector<char>& toWriteTo, int& bitIndex, bool bitToWrite);
bool readBitFromCharVector(std::vector<char>& toReadFrom, int& bitIndex);
void writeIntToDigits(std::vector<char>& buffer, int& bitIndex, int toWrite, int digits);
int readIntToDigits(std::vector<char>& buffer, int& bitIndex, int digits);

#endif
