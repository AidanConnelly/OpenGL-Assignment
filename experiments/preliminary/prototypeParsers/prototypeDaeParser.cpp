//
// Created by m on 14/10/2019.
//

#include "prototypeDaeParser.h"

xmlParsingStackMember::xmlParsingStackMember(XMLParseState state, xmlNode node) {
    this->state = state;
    this->node = node;
}
