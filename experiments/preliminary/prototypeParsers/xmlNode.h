//
// Created by aidan on 24/10/2019.
//

#ifndef OPENGLSETUP_XMLNODE_H
#define OPENGLSETUP_XMLNODE_H

#include<stdio.h>
#include<vector>
#include<string>
#include <exception>
#include <stdexcept>

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
        // printf("+");
    }

    void getValue(std::vector<char> buffer, int subStrIdx, std::string& toReturn)
    {
	    while(buffer[subStrIdx]!='\"'){
		    toReturn += buffer[subStrIdx];
		    subStrIdx++;
	    }
    }

    std::string getAttribute(std::string attributeName, std::vector<char> buffer) {
        std::string toLookFor = attributeName+"=\"";
        auto attrNameLength = toLookFor.size();
        for(int idx = startIndex;idx<(endIndex- attrNameLength);idx++){
            bool fail = false;
            for(int subStrIdx = 0;subStrIdx<attrNameLength;subStrIdx ++){
            	if(idx+subStrIdx>=buffer.size())
            	{
					throw std::invalid_argument("invalid state");
            	}
                if(buffer[idx+subStrIdx] != toLookFor[subStrIdx]){
                    fail = true;
                    break;
                }
            }
            if(!fail){
                std::string toReturn;
                getValue(buffer, idx+attrNameLength, toReturn);
                return toReturn;
            }
        }
		throw std::invalid_argument("not found");
    };

    bool hasAttribute(std::string attributeName, std::vector<char> buffer){
        std::string toLookFor = attributeName+"=\"";
        auto attrNameLength = toLookFor.size();
        for(int idx = startIndex;idx<(endIndex- attrNameLength);idx++){
            bool fail = false;
            for(int subStrIdx = 0;subStrIdx<attrNameLength;subStrIdx ++){
                if(idx+subStrIdx>=buffer.size())
                {
                    throw std::invalid_argument("invalid state");
                }
                if(buffer[idx+subStrIdx] != toLookFor[subStrIdx]){
                    fail = true;
                    break;
                }
            }
            if(!fail){
                return true;
            }
        }
        return false;
    }
};

#endif //OPENGLSETUP_XMLNODE_H
