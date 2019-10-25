//
// Created by aidan on 24/10/2019.
//

#ifndef OPENGLSETUP_XMLNODE_H
#define OPENGLSETUP_XMLNODE_H

#include<vector>
#include<string>

struct xmlNode {
    unsigned startIndex;
    unsigned endIndex;
    std::vector<xmlNode> children;
    std::string tagName;
    std::vector<float> floatsIfApplicable;
    std::vector<int> indexesIfApplicable;
};

#endif //OPENGLSETUP_XMLNODE_H
