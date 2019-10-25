//
// Created by aidan on 25/10/2019.
//

#ifndef OPENGLSETUP_XMLPARSINGSTACKMEMBER_H
#define OPENGLSETUP_XMLPARSINGSTACKMEMBER_H

#include "XMLParseState.h"
#include "xmlNode.h"

struct xmlParsingStackMember {
    xmlParsingStackMember(XMLParseState state, xmlNode* node){
        this->state = state;
        this->node = node;
    }

    //Pointer to prevent collection
    xmlNode * node;
    XMLParseState state;

    ~xmlParsingStackMember(){
    }
};

#endif //OPENGLSETUP_XMLPARSINGSTACKMEMBER_H
