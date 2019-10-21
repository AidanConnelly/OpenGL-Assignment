//
// Created by aidan on 17/10/2019.
//

#ifndef OPENGLSETUP_PROTOTYPEOBJPARSER_H
#define OPENGLSETUP_PROTOTYPEOBJPARSER_H

#include <vector>

struct OPR {

};

class prototypeObjParser {
public:
    static std::vector<OPR> parse(char * buffer, int startIndex, int endIndex){
        std::vector<OPR> toReturn;
        bool loop = true;
        while(loop){
            //Get a line, somehow cause it to be related in what's returned
        }
        return toReturn;
    }
};


#endif //OPENGLSETUP_PROTOTYPEOBJPARSER_H
