//
// Created by aidan on 24/10/2019.
//

#ifndef OPENGLSETUP_XMLNODE_H
#define OPENGLSETUP_XMLNODE_H

#include<vector>
#include<string>
#include <stdexcept>
#include <mutex>
#include <map>

static std::mutex mtx;           
static unsigned nextID = 0;

struct xmlNode {
    unsigned startIndex;
    unsigned endIndex;
    std::vector<xmlNode *> children;
    std::string tagName;
    std::vector<float> floatsIfApplicable;
	std::map<std::string, std::string> parsedAttributes;
	std::map<std::string, bool> foundAttributes;
    std::vector<int> indexesIfApplicable;
    unsigned id;

	const std::vector<char> & buffer;

    explicit xmlNode(const std::vector<char>& buffer)
	    : buffer(buffer)
    {
		this->id = nextID;
		mtx.lock();
		nextID++;
		mtx.unlock();
    }

    ~xmlNode() {
    	//todo
        // printf("+");
    }

    void getValue(int subStrIdx, std::string& toReturn)
    {
	    while(buffer[subStrIdx]!='\"'){
		    toReturn += buffer[subStrIdx];
		    subStrIdx++;
	    }
    }

	std::string getAttribute(std::string attributeName) {
		std::string toReturn = getIfHasAttribute(attributeName, "");
		return toReturn;
	};

	std::string getIfHasAttribute(std::string attributeName, std::string ifDoesntHave) {
		if (parsedAttributes.count(attributeName) == 1)
		{
			return parsedAttributes[attributeName];
		}
		if (foundAttributes.count(attributeName) == 1)
		{
			if (foundAttributes[attributeName] == false)
			{
				return ifDoesntHave;
			}
		}
		std::string toLookFor = attributeName + "=\"";
		auto attrNameLength = toLookFor.size();
		for (int idx = startIndex; idx < (endIndex - attrNameLength); idx++) {
			if (buffer[idx + attrNameLength - 1] == '>')
			{
				break;
			}
			bool fail = false;
			for (int subStrIdx = 0; subStrIdx < attrNameLength; subStrIdx++) {
				if (buffer[idx + subStrIdx] != toLookFor[subStrIdx]) {
					fail = true;
					break;
				}
			}
			if (!fail) {
				std::string toReturn;
				getValue(idx + attrNameLength, toReturn);
				foundAttributes[attributeName] = true;
				parsedAttributes[attributeName] = toReturn;
				return toReturn;
			}
		}
		foundAttributes[attributeName] = false;
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

	std::string getContents()
    {
		std::string toReturn;
		int idx = startIndex;
    	while (buffer[idx++] != '>')
		{
		}
		while (buffer[idx++] != '<')
		{
			toReturn += buffer[idx-1];
		}
		return toReturn;
    }
};

#endif //OPENGLSETUP_XMLNODE_H
