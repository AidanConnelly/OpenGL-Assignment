//
// Created by m on 14/10/2019.
//


#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

#include <utility>
#include <exception>
#include <stdexcept>
#include <functional>
//#include <d2d1helper.h>
#include <iterator>

#ifndef OPENGLSETUP_PROTOTYPEDAEPARSER_H
#define OPENGLSETUP_PROTOTYPEDAEPARSER_H

enum XMLParseState {
    Start,
    TagOpened,
    NotEndTag,
    EndTag,
    DQuote,
    SQuote
};

struct xmlNode {
    unsigned startIndex;
    unsigned endIndex;
    std::vector<xmlNode> children;
    std::string tagName;
    std::vector<float> floatsIfApplicable;
};

struct xmlParsingStackMember {
    xmlParsingStackMember(XMLParseState state, xmlNode node);

    xmlNode node;
    XMLParseState state;
};

class prototypeDaeParser {
public:
    static std::vector<xmlNode> parse(std::vector<char> buffer);

    static std::vector<xmlNode> parseNodeTagNames(std::vector<char> &buffer, const std::vector<xmlNode> &nodes);

    static std::vector<xmlNode> filterByTagName(const std::vector<xmlNode> &nodes, std::string& tagName);

private:
    static void checkForQuotes(char thisChar, int *stackPos, std::vector<xmlParsingStackMember> *stack,
                               xmlParsingStackMember *state) {
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

    static std::vector<xmlNode> parseNodes(const std::vector<char> &buffer) {
        std::vector<xmlNode> nodes = {};
        //Loop over buffer
        std::vector<xmlParsingStackMember> stack = std::vector<xmlParsingStackMember>();
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

    static std::vector<xmlNode> mapXmlNodes(const std::vector<xmlNode> input, std::function<xmlNode(xmlNode)> toMap) {
        //TODO: make parallel
        std::vector<xmlNode> toReturn;
        std::transform(input.begin(), input.end(), std::back_inserter(toReturn),
                       [&](xmlNode node) -> xmlNode { return toMap(node); });
        return toReturn;
    }

    static void parseFloatArrays(std::vector<char> buffer, const std::vector<xmlNode>& floatArrays);
};


#endif //OPENGLSETUP_PROTOTYPEDAEPARSER_H
