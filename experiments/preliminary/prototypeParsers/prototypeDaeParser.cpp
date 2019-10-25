//
// Created by m on 14/10/2019.
//

#include "prototypeDaeParser.h"
#include "stringToFloatFast.h"
#include "xmlNode.h"

xmlParsingStackMember::xmlParsingStackMember(XMLParseState state, xmlNode node) {
    this->state = state;
    this->node = node;
}

xmlNodeSet prototypeDaeParser::parseNodeTagNames(std::vector<char> &buffer, const xmlNodeSet &nodes) {
    return mapXmlNodes(nodes, [&](xmlNode node) -> xmlNode {
        int index = node.startIndex + 1;
        char character = buffer[index];
        while (character != ' ' && character != '/' && character != '>') {
            character = buffer[++index];
        }
        std::string tagName(buffer.begin() + node.startIndex + 1, buffer.begin() + index);
        node.tagName = tagName;
        return node;
    });
}

xmlNodeSet prototypeDaeParser::parse(std::vector<char> buffer) {
    xmlNodeSet nodes = parseNodes(buffer);
    xmlNodeSet nodesWithTagName = parseNodeTagNames(buffer, nodes);

    bufferParseResult result = parseLargeBuffers(buffer, nodesWithTagName);

    std::cout << result.floatArrays.size();
    return nodesWithTagName;
}

bufferParseResult prototypeDaeParser::parseLargeBuffers(const std::vector<char> &buffer, const xmlNodeSet &nodesWithTagName) {
    xmlNodeSet floatArrays = filterByTagName(nodesWithTagName, "float_array");
    floatArrays = parseFloatArrays(buffer, floatArrays);

    xmlNodeSet indexArrays = filterByTagName(nodesWithTagName, "p");
    indexArrays = parseIndexBuffer(buffer, indexArrays);

    bufferParseResult result(floatArrays, indexArrays);
    return result;
}

xmlNodeSet prototypeDaeParser::filterByTagName(const xmlNodeSet &nodes, const std::string& tagName) {
    xmlNodeSet toReturn;
    for (auto const &node: nodes) {
        if (node.tagName == tagName) {
            toReturn.push_back(node);
        }
    }
    return toReturn;
}

xmlNodeSet prototypeDaeParser::parseFloatArrays(std::vector<char> buffer, const xmlNodeSet &floatArrays) {
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

xmlNodeSet prototypeDaeParser::parseNodes(const std::vector<char> &buffer) {
    xmlNodeSet nodes = {};
    //Loop over buffer
    parseStack stack = std::vector<xmlParsingStackMember>();
    xmlParsingStackMember state = xmlParsingStackMember(Start, xmlNode());
    int stackPos = 0;
    for (unsigned i = 0; i < buffer.size(); i++) {
        char thisChar = buffer[i];
        switch (state.state) {
            case XMLParseState::Start:
                if (thisChar == '<') {
                    state.state = XMLParseState::TagOpened;
                    state.node = xmlNode();
                    state.node.startIndex = i;
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
                        state.node.endIndex = i;
                        if (state.node.endIndex < state.node.startIndex) {
                            throw std::invalid_argument("");
                        }
                        nodes.push_back(state.node);

                        if (stackPos > 0) {
                            stack[stackPos - 1].node.children.push_back(state.node);
                        }
                        state.state = XMLParseState::Start;
                        break;
                    case '>':
                        //this is a start tag
                        state.state = XMLParseState::Start;
                        stack.push_back(state);
                        stackPos++;
                        state = xmlParsingStackMember(Start, xmlNode());
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
                    state.node.endIndex = i;
                    if (state.node.endIndex < state.node.startIndex) {
                        throw std::invalid_argument("");
                    }
                    if (stackPos > 0) {
                        stack[stackPos - 1].node.children.push_back(state.node);
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

xmlNodeSet prototypeDaeParser::mapXmlNodes(const xmlNodeSet &input, std::function<xmlNode(const xmlNode &)> toMap) {
    //TODO: make parallel
    xmlNodeSet toReturn;
    std::transform(input.begin(), input.end(), std::back_inserter(toReturn),
                   [&](const xmlNode &node) -> xmlNode { return toMap(node); });
    return toReturn;
}

xmlNodeSet prototypeDaeParser::parseIndexBuffer(const std::vector<char> buffer, const xmlNodeSet &indexBuffer) {
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
