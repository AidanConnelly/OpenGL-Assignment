//
// Created by aidan on 25/10/2019.
//

#ifndef OPENGLSETUP_XMLPARSESTATE_H
#define OPENGLSETUP_XMLPARSESTATE_H

enum XMLParseState {
    Start,
    TagOpened,
    NotEndTag,
    EndTag,
    DQuote,
    SQuote
};

#endif //OPENGLSETUP_XMLPARSESTATE_H
