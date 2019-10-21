//
// Created by aidan on 17/10/2019.
//
#include "math.h"

#ifndef OPENGLSETUP_STRINGTOFLOATFAST_H
#define OPENGLSETUP_STRINGTOFLOATFAST_H

float inline parseAFloat(int *index, std::vector<char> buffer) {
    //Assume no leading chars not part of the float
    float multiplicativeCoef = 1.0f;
    float additiveCoef = 0.0f;
    if (buffer[*index] == '-') {
        multiplicativeCoef *= -1.0f;
        (*index)++;
    }
    bool isDigit = true;
    while (isDigit) {
        int i = buffer[*index] - '0';
        additiveCoef *= 10.0f;
        additiveCoef += (float) i;
        (*index)++;
        isDigit = buffer[*index] >= '0' && buffer[*index] <= '9';
    }
    float renameMe = 0.1f;
    if (buffer[*index] == '.') {
        (*index)++;
        while (buffer[*index] >= '0' && buffer[*index] <= '9') {
            int i = buffer[*index] - '0';
            additiveCoef += renameMe * ((float) i);
            renameMe *= 0.1f;
            (*index)++;
        }
    }
    bool negativeExponential = false;
    if (buffer[*index] == 'e') {
        (*index)++;
        if (buffer[*index] == '-') {
            negativeExponential = true;
            (*index)++;
        }
        int exponential = 0;
        while (buffer[*index] >= '0' && buffer[*index] <= '9') {
            int digit = buffer[*index] - '0';
            exponential *= 10;
            exponential += digit;
            (*index)++;
        }
        if (negativeExponential) {
            exponential *= -1;
        }
        //todo - pow slow?
        multiplicativeCoef *= pow(10, exponential);
    }
    return additiveCoef * multiplicativeCoef;
}

#endif //OPENGLSETUP_STRINGTOFLOATFAST_H
