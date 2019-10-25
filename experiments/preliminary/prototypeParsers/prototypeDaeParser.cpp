//
// Created by m on 14/10/2019.
//

#include "prototypeDaeParser.h"
#include "stringToFloatFast.h"
#include "xmlNode.h"
#include "bufferParseResult.h"
#include "typedefs.h"
#include "XMLParseState.h"

void prototypeDaeParser::parseNodeTagNames(std::vector<char> &buffer, xmlNodeStore &nodes) {
    alterXmlNodes(nodes, [&](xmlNode *node) -> xmlNode {
        int index = node->startIndex + 1;
        char character = buffer[index];
        while (character != ' ' && character != '/' && character != '>') {
            character = buffer[++index];
        }
        std::string tagName(buffer.begin() + node->startIndex + 1, buffer.begin() + index);
        node->tagName = tagName;
        return *node;
    });
}

xmlNodeVector prototypeDaeParser::parse(std::vector<char> buffer) {
    xmlNodeStore nodes = parseNodes(buffer);
    parseNodeTagNames(buffer, nodes);
    xmlNodeVector asVec;
    std::for_each(nodes.begin(), nodes.end(), [&](xmlNode *node) -> void {
                      asVec.push_back(*node);
                  }
    );
    bufferParseResult result = parseLargeBuffers(buffer, asVec);
    auto nodeParseResults = parseNodeTags(buffer, asVec);
    auto meshParseResults = parseMeshTags(buffer, asVec);
    std::cout << result.floatArrays.size();
    return asVec;
}

std::vector<parseNodeTagsResult> prototypeDaeParser::parseNodeTags(std::vector<char> buffer, xmlNodeVector nodes) {
    std::vector<parseNodeTagsResult> toReturn;
    //todo find all node tags, inside: parse matrix tag, parse instance_geometry child tags
    auto nodeTags = filterByTagName(nodes, "node");
    mapXmlNodes(nodeTags, [&](xmlNode node) -> xmlNode {
        std::vector<xmlNode *> children = node.children;
        auto matrix = getSoleByTag(children, "matrix");
        auto i_g = filterByTagName(children, "instance_geometry");
        toReturn.push_back(parseNodeTagsResult(matrix,i_g));
        return node;
    });
}

xmlNodeVector prototypeDaeParser::parseMeshTags(std::vector<char> buffer, xmlNodeVector nodes) {
    //todo all mesh tags, process those
    filterByTagName(nodes, "mesh");
}

xmlNode prototypeDaeParser::getSoleByTag(const xmlNodeStore& toSearchIn, std::string toSearchFor){
    auto x = filterByTagName(toSearchIn, toSearchFor);
    if(x.size()!=1){
        throw std::invalid_argument("not single");
    }
    return x[0];
}

bufferParseResult prototypeDaeParser::parseLargeBuffers(const std::vector<char> &buffer, const xmlNodeVector &nodesWithTagName) {
    xmlNodeVector floatArrays = filterByTagName(nodesWithTagName, "float_array");
    floatArrays = parseFloatArrays(buffer, floatArrays);

    xmlNodeVector indexArrays = filterByTagName(nodesWithTagName, "p");
    indexArrays = parseIndexBuffer(buffer, indexArrays);

    bufferParseResult result(floatArrays, indexArrays);
    return result;
}

xmlNodeVector prototypeDaeParser::filterByTagName(const xmlNodeVector &nodes, const std::string &tagName) {
    xmlNodeVector toReturn;
    for (auto const &node: nodes) {
        if (node.tagName == tagName) {
            toReturn.push_back(node);
        }
    }
    return toReturn;
}

xmlNodeVector prototypeDaeParser::filterByTagName(const std::vector<xmlNode *> &nodes, const std::string &tagName) {
    xmlNodeVector toReturn;
    for (auto const &node: nodes) {
        if (node->tagName == tagName) {
            toReturn.push_back(*node);
        }
    }
    return toReturn;
}

xmlNodeVector prototypeDaeParser::parseFloatArrays(std::vector<char> buffer, const xmlNodeVector &floatArrays) {
    return mapXmlNodes(floatArrays, [&](xmlNode node) -> xmlNode {
        int index = node.startIndex;
        while (buffer[index] != '>') {
            index++;
        }
        std::vector<float> floats;
        do {
            index++;
            float f = parseAFloat(&index, buffer);
            floats.push_back(f);
        } while (buffer[index] == ' ');
        node.floatsIfApplicable = floats;
        return node;
    });

}

void prototypeDaeParser::checkForQuotes(char thisChar, int *stackPos, parseStack *stack, xmlParsingStackMember *state) {
    int newStackPos;
    switch (thisChar) {
        case '"':
            (*stack).push_back(*state);
            newStackPos = *stackPos + 1;
            *stackPos = newStackPos;
            (*state).state = XMLParseState::DQuote;
            break;
        case '\'':
            (*stack).push_back(*state);
            newStackPos = *stackPos + 1;
            *stackPos = newStackPos;
            (*state).state = XMLParseState::SQuote;
            break;
        default:
            break;
    }
}


xmlNodeStore prototypeDaeParser::parseNodes(const std::vector<char> &buffer) {
    xmlNodeStore nodes = {};
    //Loop over buffer
    parseStack stack = std::vector<xmlParsingStackMember>();
    auto *node = new xmlNode();
    xmlParsingStackMember state = xmlParsingStackMember(Start, node);
    int stackPos = 0;
    for (unsigned i = 0; i < buffer.size(); i++) {
        char thisChar = buffer[i];
        switch (state.state) {
            case XMLParseState::Start:
                if (thisChar == '<') {
                    state.state = XMLParseState::TagOpened;
                    state.node = new xmlNode();
                    state.node->startIndex = i;
                }
                break;
            case XMLParseState::TagOpened:
                if (thisChar == '/') {
                    state.state = XMLParseState::EndTag;
                } else if (thisChar == '?') {
                    state.state = XMLParseState::Start;
                } else {
                    state.state = XMLParseState::NotEndTag;
                }
                break;
            case XMLParseState::NotEndTag:
                switch (thisChar) {
                    case '/':
                        //Next char will be ">", which shouldn't be a problem for the parser
                        state.node->endIndex = i;
                        if (state.node->endIndex < state.node->startIndex) {
                            throw std::invalid_argument("");
                        }
                        nodes.push_back(state.node);

                        if (stackPos > 0) {
                            stack[stackPos - 1].node->children.push_back(state.node);
                        }
                        state.state = XMLParseState::Start;
                        break;
                    case '>':
                        //this is a start tag
                        state.state = XMLParseState::Start;
                        stack.push_back(state);
                        stackPos++;
                        state = xmlParsingStackMember(Start, new xmlNode());
                        break;
                    default:
                        checkForQuotes(thisChar, &stackPos, &stack, &state);
                        break;
                }
                break;
            case XMLParseState::EndTag:
                if (thisChar == '>') {
                    stackPos--;
                    state = stack[stackPos];
                    stack.pop_back();
                    state.node->endIndex = i;
                    if (state.node->endIndex < state.node->startIndex) {
                        throw std::invalid_argument("");
                    }
                    if (stackPos > 0) {
                        stack[stackPos - 1].node->children.push_back(state.node);
                    }
                    nodes.push_back(state.node);
                } else {
                    checkForQuotes(thisChar, &stackPos, &stack, &state);
                }
                break;
            case XMLParseState::DQuote:
                if (thisChar == '"') {
                    stackPos--;
                    state = stack[stackPos];
                    stack.pop_back();
                }
                break;
            case XMLParseState::SQuote:
                if (thisChar == '\'') {
                    stackPos--;
                    state = stack[stackPos];
                    stack.pop_back();
                }
                break;
        }
    }

    return nodes;
}

xmlNodeVector prototypeDaeParser::mapXmlNodes(const xmlNodeVector &input, std::function<xmlNode(const xmlNode &)> toMap) {
    //TODO: make parallel
    xmlNodeVector toReturn;
    std::transform(input.begin(), input.end(), std::back_inserter(toReturn),
                   [&](const xmlNode &node) -> xmlNode { return toMap(node); });
    return toReturn;
}

void prototypeDaeParser::alterXmlNodes(xmlNodeStore &input, std::function<void(xmlNode *)> toMap) {
    //TODO: make parallel
    for (auto &i : input) {
        toMap(i);
    }
}

xmlNodeVector prototypeDaeParser::parseIndexBuffer(const std::vector<char> buffer, const xmlNodeVector &indexBuffer) {
    return mapXmlNodes(indexBuffer, [&](xmlNode node) -> xmlNode {
        int index = node.startIndex;
        while (buffer[index] != '>') {
            index++;
        }
        std::vector<int> indexes;
        do {
            index++;
            int ind = parseAnInt(&index, buffer);
            indexes.push_back(ind);
        } while (buffer[index] == ' ');
        node.indexesIfApplicable = indexes;
        return node;
    });
}

parseNodeTagsResult::parseNodeTagsResult(xmlNode node, std::vector<xmlNode> vector) {
    //todo - fill me in!
}
