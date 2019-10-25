//
// Created by m on 14/10/2019.
//


#ifndef OPENGLSETUP_PROTOTYPEDAEPARSER_H
#define OPENGLSETUP_PROTOTYPEDAEPARSER_H

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
#include "bufferParseResult.h"
#include "typedefs.h"
#include "glm/glm.hpp"

struct parseNodeTagsResult {
    parseNodeTagsResult(xmlNode node, std::vector<xmlNode> vector);

    glm::mat4 transform;
    std::vector<std::string> IDs;
};

class prototypeDaeParser {
public:
    static xmlNodeVector parse(std::vector<char> buffer);

private:
    static void parseNodeTagNames(std::vector<char> &buffer, xmlNodeStore &nodes);

    static xmlNodeVector filterByTagName(const xmlNodeVector &nodes, const std::string &tagName);

    static xmlNodeVector filterByTagName(const std::vector<xmlNode *> &nodes, const std::string &tagName);

    static void checkForQuotes(char thisChar, int *stackPos, parseStack *stack, xmlParsingStackMember *state);

    static xmlNodeStore parseNodes(const std::vector<char> &buffer);

    static xmlNodeVector mapXmlNodes(const xmlNodeVector &input, std::function<xmlNode(const xmlNode &)> toMap);

    static void alterXmlNodes(xmlNodeStore &input, std::function<void(xmlNode *)> toMap);

    static xmlNodeVector parseFloatArrays(std::vector<char> buffer, const xmlNodeVector &floatArrays);

    static xmlNodeVector parseIndexBuffer(std::vector<char> buffer, const xmlNodeVector &indexBuffer);

    static bufferParseResult parseLargeBuffers(const std::vector<char> &buffer, const xmlNodeVector &nodesWithTagName);

    static xmlNodeVector parseMeshTags(std::vector<char> buffer, xmlNodeVector nodes);

    static std::vector<parseNodeTagsResult> parseNodeTags(std::vector<char> buffer, xmlNodeVector nodes);

    static xmlNode getSoleByTag(const xmlNodeStore& toSearchIn, std::string toSearchFor);

};

#endif //OPENGLSETUP_PROTOTYPEDAEPARSER_H
