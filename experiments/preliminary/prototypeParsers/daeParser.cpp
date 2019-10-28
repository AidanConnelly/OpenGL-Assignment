//
// Created by m on 14/10/2019.
//

#include "daeParser.h"
#include "stringToFloatFast.h"
#include "xmlNode.h"
#include "bufferParseResult.h"
#include "typedefs.h"
#include "XMLParseState.h"

//#include <glm/gtc/type_ptr.hpp>

void daeParser::parseNodeTagNames(std::vector<char> &buffer, xmlNodeStore &nodes) {
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

xmlNodeVector daeParser::parse(std::vector<char> buffer) {
    xmlNodeStore nodes = parseNodes(buffer);
    parseNodeTagNames(buffer, nodes);
    xmlNodeVector asVec;
    std::for_each(nodes.begin(), nodes.end(), [&](xmlNode *node) -> void {
                      asVec.push_back(*node);
                  }
    );
    bufferParseResult result = parseLargeBuffers(buffer, asVec);
    auto nodeParseResults = parseNodeTags(buffer, asVec);
    auto meshParseResults = parseMeshTags(buffer, asVec, result);
    //todo apply transforms to instance_gemetries???
    std::cout << result.floatArrays.size();
    return asVec;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-use-emplace"

std::vector<parseNodeTagsResult> daeParser::parseNodeTags(std::vector<char> buffer, xmlNodeVector nodes) {
    std::vector<parseNodeTagsResult> toReturn;
    auto nodeTags = filterByTagName(nodes, "node");
    mapXmlNodes(nodeTags, [&](xmlNode node) -> xmlNode {
        std::vector<xmlNode *> children = node.children;
        auto matrix = getSoleByTag(children, "matrix");
        auto i_g = filterByTagName(children, "instance_geometry");
        toReturn.push_back(parseNodeTagsResult(matrix, i_g, buffer));
        return node;
    });
    return toReturn;
}

#pragma clang diagnostic pop

std::vector<meshParseResult> daeParser::parseMeshTags(std::vector<char> buffer, xmlNodeVector nodes, bufferParseResult largeBuffers) {
    std::vector<meshParseResult> results;
    std::vector<xmlNode> tags = filterByTagName(nodes, "geometry");
    for (auto &geometryTag : tags) {
        meshParseResult thisResult = meshParseResult();
        std::string id = geometryTag.getAttribute("id", buffer);
        xmlNode tag = *geometryTag.children[0];
        if (tag.tagName != "mesh") {
            throw std::invalid_argument("unexpected state");
        }
        thisResult.meshID = id;
        auto triangles = filterByTagName(tag.children, "triangles");
        for (auto &triangleTag : triangles) {
            xmlNode pTag = getSoleByTag(triangleTag.children, "p");
            auto vertexInputTag = getSoleByAttrib(triangleTag.children, "semantic", "VERTEX", buffer);
            auto normalInputTag = getSoleByAttrib(triangleTag.children, "semantic", "NORMAL", buffer);
            auto tCoordInputTag = getSoleByAttrib(triangleTag.children, "semantic", "TEXCOORD", buffer);
            int vertexOffset = stoi(vertexInputTag.getAttribute("offset", buffer));
            int normalOffset = stoi(normalInputTag.getAttribute("offset", buffer));
            int tCoordOffset = stoi(tCoordInputTag.getAttribute("offset", buffer));
            int maxOffset = 0;
            auto inputTags = filterByTagName(triangleTag.children, "input");
            for (auto &node: inputTags) {
                int offset = stoi(node.getAttribute("offset", buffer));
                if (offset > maxOffset) {
                    maxOffset = offset;
                }
            }

            xmlNode thisTriangleIndexArray;
            for (auto &indexArray: largeBuffers.indexArrays) {
                if (indexArray.id == pTag.id) {
                    thisTriangleIndexArray = indexArray;
                }
            }

            int vertexCount = thisTriangleIndexArray.indexesIfApplicable.size() / maxOffset;
            int triangleCount = vertexCount / 3;
            if (triangleCount * (maxOffset + 1) * 3 != thisTriangleIndexArray.indexesIfApplicable.size()) {
                throw std::invalid_argument("wrong number of indexes");
            }

            const std::string &verticesID = vertexInputTag.getAttribute("source", buffer);
            auto vertexTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(verticesID), buffer);
            const std::string &inputID = vertexTag.children[0]->getAttribute("source", buffer);
            auto positionSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(inputID), buffer);

            const std::string &normalSourceId = normalInputTag.getAttribute("source", buffer);
            auto normalSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(normalSourceId), buffer);

            const std::string &tCoordSourceId = tCoordInputTag.getAttribute("source", buffer);
            auto tCoordSourceTag = getSoleByAttrib(tag.children, "id", removeLeadingHash(tCoordSourceId), buffer);

            for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++) {
//region Region_1
                for (int i = 0; i < 3; i++) {
                    vertexDef building{};

                    unsigned vertexIndex = 3 * triangleIndex + i;
                    unsigned positnIndex = thisTriangleIndexArray.indexesIfApplicable[(maxOffset+1) * vertexIndex + vertexOffset];
                    unsigned normalIndex = thisTriangleIndexArray.indexesIfApplicable[(maxOffset+1) * vertexIndex + normalOffset];
                    unsigned tCoordIndex = thisTriangleIndexArray.indexesIfApplicable[(maxOffset+1) * vertexIndex + tCoordOffset];

                    const xmlNode &positnFloat = soleLargeFloatChild(positionSourceTag, largeBuffers);
                    //vertex X
                    building.x = getViaParam("X", positionSourceTag, positnFloat, positnIndex, buffer);
                    //vertex Y
                    building.y = getViaParam("Y", positionSourceTag, positnFloat, positnIndex, buffer);
                    //vertex Z
                    building.z = getViaParam("Z", positionSourceTag, positnFloat, positnIndex, buffer);

                    const xmlNode &nrmlFloat = soleLargeFloatChild(normalSourceTag, largeBuffers);
                    //vertexNormal X
                    building.nX = getViaParam("X", normalSourceTag, nrmlFloat, normalIndex, buffer);
                    //vertexNormal Y
                    building.nY = getViaParam("Y", normalSourceTag, nrmlFloat, normalIndex, buffer);
                    //vertexNormal Z
                    building.nZ = getViaParam("Z", normalSourceTag, nrmlFloat, normalIndex, buffer);

                    const xmlNode &tCoordArray = soleLargeFloatChild(tCoordSourceTag, largeBuffers);
                    //texCoord U
                    building.u = getViaParam("S", tCoordSourceTag, tCoordArray, tCoordIndex, buffer);
                    //texCoord V
                    building.v = getViaParam("T", tCoordSourceTag, tCoordArray, tCoordIndex, buffer);

                    thisResult.vertexes.push_back(building);
                }
                triangle constructed{};
                constructed.v1i = thisResult.vertexes.size() - 3;
                constructed.v2i = thisResult.vertexes.size() - 2;
                constructed.v3i = thisResult.vertexes.size() - 1;
                thisResult.triangles.push_back(constructed);
            }
//endregion Region_1


            //Can now construct vertex buffer and vertex index buffer from p tag data
        }
//region Region_2

        //Find <p>,
        //Find semantic = VERTEX <vertices> ~> <input> child ~> <source>
        //Find semantic = NORMAL <source>
        //Find semantic = TEXCOORD <source>

        //<source>
        //<float_array>
        //<technique_common>
        //From = Index of <param NUM>
        //To = NUM
        //endregion Region_2

        results.push_back(thisResult);

    }
    return results;
}

xmlNode daeParser::soleLargeFloatChild(const xmlNode &parent, bufferParseResult bpr) {
    for (auto &b: parent.children) {
        for (auto &a: bpr.floatArrays) {
            unsigned aId = a.id;
            unsigned bId = b->id;
            if (aId == bId) {
                return a;
            }
        }
    }
    throw std::invalid_argument("no large float child found");
}

float daeParser::getViaParam(std::string toGet, xmlNode sourceTag, xmlNode parsedFloatArray, unsigned index, std::vector<char> buffer) {
    auto technique_common = getSoleByTag(sourceTag.children, "technique_common");
    auto accessor = getSoleByTag(technique_common.children, "accessor");
    int count = stoi(accessor.getAttribute("count", buffer));
    int stride = stoi(accessor.getAttribute("stride", buffer));
    int offset = 0;
    if (accessor.hasAttribute("offset", buffer)) {
        const std::string &asStr = accessor.getAttribute("offset", buffer);
        offset = stoi(asStr);
    }

    std::vector<xmlNode> params = filterByTagName(accessor.children, "param");
    xmlNode param;
    int idx = -1;
    for (int i = 0; i < params.size(); i++) {
        if (params[i].getAttribute("name", buffer) == toGet) {
            param = params[i];
            idx = i + offset;
        }
    }
    if (idx == -1) {
        throw std::invalid_argument("todo");
    }
    return parsedFloatArray.floatsIfApplicable[index * stride + idx];
}

xmlNode daeParser::getSoleByAttrib(const xmlNodeStore &lookIn, std::string attrib, std::string value,
                                   std::vector<char> buffer) {
    for (auto &node:lookIn) {
        if (node->getAttribute(attrib, buffer) == value) {
            return *node;
        }
    }
    throw std::invalid_argument("no match");
}

xmlNode daeParser::getSoleByTag(const xmlNodeStore &toSearchIn, std::string toSearchFor) {
    auto x = filterByTagName(toSearchIn, toSearchFor);
    if (x.size() != 1) {
        throw std::invalid_argument("not single");
    }
    return x[0];
}

bufferParseResult daeParser::parseLargeBuffers(const std::vector<char> &buffer, const xmlNodeVector &nodesWithTagName) {
    xmlNodeVector floatArrays = filterByTagName(nodesWithTagName, "float_array");
    floatArrays = parseFloatArrays(buffer, floatArrays);

    xmlNodeVector indexArrays = filterByTagName(nodesWithTagName, "p");
    indexArrays = parseIndexBuffer(buffer, indexArrays);

    bufferParseResult result(floatArrays, indexArrays);
    return result;
}

xmlNodeVector daeParser::filterByTagName(const xmlNodeVector &nodes, const std::string &tagName) {
    xmlNodeVector toReturn;
    for (auto const &node: nodes) {
        if (node.tagName == tagName) {
            toReturn.push_back(node);
        }
    }
    return toReturn;
}

xmlNodeVector daeParser::filterByTagName(const std::vector<xmlNode *> &nodes, const std::string &tagName) {
    xmlNodeVector toReturn;
    for (auto const &node: nodes) {
        if (node->tagName == tagName) {
            toReturn.push_back(*node);
        }
    }
    return toReturn;
}

xmlNodeVector daeParser::parseFloatArrays(std::vector<char> buffer, const xmlNodeVector &floatArrays) {
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

void daeParser::checkForQuotes(char thisChar, int *stackPos, parseStack *stack, xmlParsingStackMember *state) {
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


xmlNodeStore daeParser::parseNodes(const std::vector<char> &buffer) {
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

xmlNodeVector daeParser::mapXmlNodes(const xmlNodeVector &input, std::function<xmlNode(const xmlNode &)> toMap) {
    //TODO: make parallel
    xmlNodeVector toReturn;
    std::transform(input.begin(), input.end(), std::back_inserter(toReturn),
                   [&](const xmlNode &node) -> xmlNode { return toMap(node); });
    return toReturn;
}

void daeParser::alterXmlNodes(xmlNodeStore &input, std::function<void(xmlNode *)> toMap) {
    //TODO: make parallel
    for (auto &i : input) {
        toMap(i);
    }
}

xmlNodeVector daeParser::parseIndexBuffer(const std::vector<char> buffer, const xmlNodeVector &indexBuffer) {
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

std::string daeParser::removeLeadingHash(const std::string &toRemove) {
    return toRemove.substr(1, toRemove.size() - 1);
}

parseNodeTagsResult::parseNodeTagsResult(xmlNode matrix, xmlNodeVector instance_geometryTags, std::vector<char> buffer) {
    int index = matrix.startIndex;
    do {
        index++;
    } while ('>' != buffer[index]);
    index++;
    int floatIndex = 0;
    float floatArray[16];
    while (buffer[index] >= '0' && buffer[index] <= '9') {
        floatArray[floatIndex] = parseAFloat(&index, buffer);
        index++;
        floatIndex++;
    }
//    this->transform = glm::make_mat4(floatArray);

    auto beginning = instance_geometryTags.begin();
    auto ending = instance_geometryTags.end();
    auto forEach = [&](xmlNode node) -> void {
        auto IDs = this->IDs;
        auto id = node.getAttribute("url", buffer);
        IDs.insert(id);
    };
    std::for_each(beginning, ending, forEach);
}
