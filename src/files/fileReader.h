//
// Created by m on 11/10/2019.
//

#ifndef OPENGLSETUP_FILETHROUGHPUT_H
#define OPENGLSETUP_FILETHROUGHPUT_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <functional>
#include <vector>
#include "fs.h"

inline std::string separator()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

class fileReader {
public:
    static std::vector<char> read(std::string file) {
        std::vector<char> readed;
        if(filesystem::exists(filesystem::path(file))) {
			std::ifstream myfile(file);
            const int fileSize = filesystem::file_size(file);
			readed.resize( fileSize);
            myfile.read(&readed[0], fileSize);
            myfile.close();
        }
        return readed;
    }

	static void printFunctionExecutionTime(const std::function<void(void)> &functionPointer) {
		auto start = std::chrono::high_resolution_clock::now();
        functionPointer();
		auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
		std::cout << diff.count() << std::endl;
		std::cout << std::chrono::duration <double, std::nano>(diff).count() << " ns" << std::endl;
		std::cout << std::chrono::duration <double, std::micro>(diff).count() << " us" << std::endl;
		std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
    }
};



#endif //OPENGLSETUP_FILETHROUGHPUT_H
