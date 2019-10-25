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

typedef std::vector<xmlNode> xmlNodeSet;

struct bufferParseResult {
    bufferParseResult(xmlNodeSet floatArrays, xmlNodeSet indexArrays) {
        this->floatArrays = std::move(floatArrays);
        this->indexArrays = std::move(indexArrays);
    }

    xmlNodeSet floatArrays;
    xmlNodeSet indexArrays;
};

enum XMLParseState {
    Start,
    TagOpened,
    NotEndTag,
    EndTag,
    DQuote,
    SQuote
};

struct xmlParsingStackMember {
    xmlParsingStackMember(XMLParseState state, xmlNode node);

    xmlNode node;
    XMLParseState state;
};

typedef std::vector<xmlParsingStackMember> parseStack;


class prototypeDaeParser {
public:
    static xmlNodeSet parse(std::vector<char> buffer);

private:
    static xmlNodeSet parseNodeTagNames(std::vector<char> &buffer, const xmlNodeSet &nodes);

    static xmlNodeSet filterByTagName(const xmlNodeSet &nodes, const std::string& tagName);

    static void checkForQuotes(char thisChar, int *stackPos, parseStack *stack, xmlParsingStackMember *state);

    static xmlNodeSet parseNodes(const std::vector<char> &buffer);

    static xmlNodeSet mapXmlNodes(const xmlNodeSet &input, std::function<xmlNode(const xmlNode&)> toMap);

    static xmlNodeSet parseFloatArrays(std::vector<char> buffer, const xmlNodeSet &floatArrays);

    static xmlNodeSet parseIndexBuffer(std::vector<char> buffer, const xmlNodeSet &indexBuffer);

    static bufferParseResult parseLargeBuffers(const std::vector<char> &buffer, const xmlNodeSet &nodesWithTagName);
};


#endif //OPENGLSETUP_PROTOTYPEDAEPARSER_H
