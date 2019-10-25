//
// Created by aidan on 24/10/2019.
//

#ifndef OPENGLSETUP_XMLNODE_H
#define OPENGLSETUP_XMLNODE_H

#include "prototypeDaeParser.h"

struct xmlNode {
    unsigned startIndex;
    unsigned endIndex;
    std::vector<xmlNode> children;
    std::string tagName;
    std::vector<float> floatsIfApplicable;
    std::vector<int> indexesIfApplicable;
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

#endif //OPENGLSETUP_XMLNODE_H
