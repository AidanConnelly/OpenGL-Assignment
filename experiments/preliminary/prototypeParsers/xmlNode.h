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
        std::string toLookFor = attributeName+"=\"";
        for(int idx = startIndex;idx<(endIndex-toLookFor.size());idx++){
            bool fail = false;
            for(int subStrIdx = 0;subStrIdx<toLookFor.size();subStrIdx ++){
                if(buffer[idx+subStrIdx] != toLookFor[subStrIdx]){
                    fail = true;
                    break;
                }
            }
            if(!fail){
                std::string toReturn;
                int subStrIdx = idx+toLookFor.size();
                while(buffer[subStrIdx]!='\"'){
                    toReturn += buffer[subStrIdx];
                    subStrIdx++;
                }
                return toReturn;
            }
        }
    };
};

#endif //OPENGLSETUP_XMLNODE_H
