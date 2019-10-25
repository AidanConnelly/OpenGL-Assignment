//
// Created by aidan on 25/10/2019.
//

#ifndef OPENGLSETUP_BUFFERPARSERESULT_H
#define OPENGLSETUP_BUFFERPARSERESULT_H

#include "typedefs.h"

struct bufferParseResult {
    bufferParseResult(xmlNodeVector floatArrays, xmlNodeVector indexArrays) {
        this->floatArrays = std::move(floatArrays);
        this->indexArrays = std::move(indexArrays);
    }

    xmlNodeVector floatArrays;
    xmlNodeVector indexArrays;
};

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <exception>
#include <stdexcept>
#include <functional>
#include <iterator>
#include "xmlNode.h"
#include "xmlParsingStackMember.h"
#include "typedefs.h"

#endif //OPENGLSETUP_BUFFERPARSERESULT_H
