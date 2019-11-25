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
#include <filesystem>
#include "../prototypeParsers/daeParser.h"

inline std::string separator()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

class fileThroughput {
public:
    static void getLineByLine() {
        for (int i = 0; i < 1; i++) {
            int nLines = 0;
            std::ifstream myfile("resources\\largeMesh.obj");
            std::string line;
            while (getline(myfile, line)) {
                nLines += line.length();
            }
            std::cout << nLines << std::endl;
            myfile.close();
        }
    }

    static std::vector<char> getBytes(std::string file) {
        std::vector<char> readed;
        if(exists(std::filesystem::path(file))) {
            std::cout << readed.size() << std::endl;
            for (int i = 0; i < 1; i++) {

                //std::ifstream myfile("resources"+separator()+"largish.dae");
                std::ifstream myfile(file);

                int timesRead = 0;

                //512 KB chunks for fast opening of smaller files
                const int maxNChars = 512 * 1024;
                char buffer[maxNChars]; // create a buffer
                while (!myfile.eof()) {
                    timesRead++;
                    myfile.read(buffer, maxNChars); // read to buffer
                    int charsReaded = myfile.gcount();
                    readed.insert(readed.end(), buffer, buffer + charsReaded);
                }
                std::cout << timesRead << std::endl;
                myfile.close();
            }
        }
        return readed;
    }

    static void read4objs(){
        // std::vector<char> toParse = fileThroughput::getBytes();
//        xmlNodeVector results = daeParser::parse(toParse);
    }

    static void runExperiment() {
        for (int i = 0; i < 1; i++) {
            printFunctionExecutionTime(read4objs);
//            printFunctionExecutionTime(getBytes);
//            printFunctionExecutionTime(getLineByLine);
        }
    }

    static void printFunctionExecutionTime(const std::function<void(void)> &functionPointer) {
		auto start = std::chrono::high_resolution_clock::now();
        functionPointer();
		auto end = std::chrono::high_resolution_clock::now();
//        auto diff = end - start;
        auto diff = end - start;
		std::cout << diff.count() << std::endl;
		std::cout << std::chrono::duration <double, std::nano>(diff).count() << " ns" << std::endl;
		std::cout << std::chrono::duration <double, std::micro>(diff).count() << " us" << std::endl;
		std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
//        auto miliSeconds = duration * 1000;
//        std::cout << miliSeconds << "ms\n";
    }
};



#endif //OPENGLSETUP_FILETHROUGHPUT_H
