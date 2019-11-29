//
// Created by aidan on 25/10/2019.
//

#ifndef OPENGLSETUP_BUFFERPARSERESULT_H
#define OPENGLSETUP_BUFFERPARSERESULT_H

#include "typedefs.h"

struct bufferParseResult {
    bufferParseResult(xmlNodeStore floatArrays, xmlNodeStore indexArrays) {
        this->floatArrays = std::move(floatArrays);
        this->indexArrays = std::move(indexArrays);
    }

	xmlNodeStore floatArrays;
	xmlNodeStore indexArrays;
};

#endif //OPENGLSETUP_BUFFERPARSERESULT_H
