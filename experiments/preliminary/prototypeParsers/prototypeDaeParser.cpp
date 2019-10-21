//
// Created by m on 14/10/2019.
//

#include "prototypeDaeParser.h"
#include "stringToFloatFast.h"

xmlParsingStackMember::xmlParsingStackMember(XMLParseState state, xmlNode node) {
    this->state = state;
    this->node = node;
}

std::vector<xmlNode> prototypeDaeParser::parseNodeTagNames(std::vector<char> &buffer, const std::vector<xmlNode> &nodes) {
    return mapXmlNodes(nodes, [&](xmlNode node) -> xmlNode {
        int index = node.startIndex + 1;
        char character = buffer[index];
        while(character != ' ' && character != '/' && character != '>'){
            character = buffer[++index];
        }
        std::string tagName(buffer.begin()+node.startIndex+1,buffer.begin()+index);
        node.tagName = tagName;
        return node;
    });
}

std::vector<xmlNode> prototypeDaeParser::parse(std::vector<char> buffer) {
    std::vector<xmlNode> nodes = parseNodes(buffer);
    std::vector<xmlNode> nodesWithTagName = parseNodeTagNames(buffer, nodes);
    std::string floatArray = "float_array";
    std::vector<xmlNode> floatArrays = filterByTagName(nodesWithTagName, floatArray);
    parseFloatArrays(buffer, floatArrays);
    return nodesWithTagName;
}

std::vector<xmlNode> prototypeDaeParser::filterByTagName(const std::vector<xmlNode> &nodes,std::string& tagName) {
    std::vector<xmlNode> toReturn;
    for(auto const & node: nodes){
        if(node.tagName == tagName){
            toReturn.push_back(node);
        }
    }
    return toReturn;
}

void prototypeDaeParser::parseFloatArrays(std::vector<char> buffer, const std::vector<xmlNode>& floatArrays) {
    mapXmlNodes(floatArrays, [&](xmlNode node)-> xmlNode {
        int index = node.startIndex;
        while(buffer[index]!='>'){
            index++;
        }
        std::vector<float> floats;
        do
        {
            index++;
            float f = parseAFloat(&index,buffer);
            floats.push_back(f);
        }
        while( buffer[index] ==' ');
        node.floatsIfApplicable = floats;
        return node;
    });

}
