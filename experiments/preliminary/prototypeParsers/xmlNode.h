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

	const std::vector<char> *buffer;

    xmlNode(const std::vector<char>*buffer){
		this->buffer = buffer;
        this->id = nextID;
        nextID++;
    }

    ~xmlNode() {
        // printf("+");
    }

    void getValue(int subStrIdx, std::string& toReturn)
    {
	    while((*buffer)[subStrIdx]!='\"'){
		    toReturn += (*buffer)[subStrIdx];
		    subStrIdx++;
	    }
    }

	std::string getAttribute(std::string attributeName) {
		std::string toReturn = getIfHasAttribute(attributeName, "string not found");
    	if(toReturn=="string not found")
    	{
			throw std::invalid_argument("todo");
    	}
		return toReturn;
	};
	
	std::string getIfHasAttribute(std::string attributeName, std::string ifDoesntHave) {
		std::string toLookFor = attributeName + "=\"";
		auto attrNameLength = toLookFor.size();
		for (int idx = startIndex; idx<(endIndex - attrNameLength); idx++) {
			if((*buffer)[idx+attrNameLength-1]=='>')
			{
				break;
			}
			bool fail = false;
			for (int subStrIdx = 0; subStrIdx<attrNameLength; subStrIdx++) {
				if (idx + subStrIdx >= buffer->size())
				{
					throw std::invalid_argument("invalid state");
				}
				if ((*buffer)[idx + subStrIdx] != toLookFor[subStrIdx]) {
					fail = true;
					break;
				}
			}
			if (!fail) {
				std::string toReturn;
				getValue(idx + attrNameLength, toReturn);
				return toReturn;
			}
		}
		return ifDoesntHave;
	};

    bool hasAttribute(std::string attributeName){
		std::string val = getIfHasAttribute(attributeName, "string not found");
		if (val == "string not found")
		{
			return false;
		}
		return true;
    }
};

#endif //OPENGLSETUP_XMLNODE_H
