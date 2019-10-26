//
// Created by aidan on 24/10/2019.
//

#ifndef OPENGLSETUP_XMLNODE_H
#define OPENGLSETUP_XMLNODE_H

#include<stdio.h>
#include<vector>
#include<string>

static unsigned nextID = 0;

struct xmlNode {
    unsigned startIndex;
    unsigned endIndex;
    std::vector<xmlNode *> children;
    std::string tagName;
    std::vector<float> floatsIfApplicable;
    std::vector<int> indexesIfApplicable;
    unsigned id;

    xmlNode(){
        this->id = nextID;
        nextID++;
    }

    ~xmlNode() {
        printf("+");
    }

    std::string getAttribute(std::string attributeName, std::vector<char> buffer) {
        //todo
        return attributeName;
    };
};

#endif //OPENGLSETUP_XMLNODE_H
