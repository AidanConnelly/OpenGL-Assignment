//
// Created by m on 11/10/2019.
//

#ifndef OPENGLSETUP_FILETHROUGHPUT_H
#define OPENGLSETUP_FILETHROUGHPUT_H


#include <iostream>
#include <fstream>
#include <time.h>
#include <chrono>
#include <functional>
#include <vector>
#include "../prototypeParsers/prototypeDaeParser.h"
#include "../prototypeParsers/typedefs.h"

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

    static std::vector<char> getBytes() {
        std::vector<char> readed;
        std::cout << readed.size() << std::endl;
        for (int i = 0; i < 1; i++) {

            std::ifstream myfile("C:\\Users\\aidan\\Documents\\soft356a3\\resources\\singleTriangle.dae");

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
        return readed;
    }

    static void read4objs(){
        std::vector<char> toParse = fileThroughput::getBytes();
        xmlNodeVector results = prototypeDaeParser::parse(toParse);
    }

    static void runExperiment() {
        for (int i = 0; i < 9; i++) {
            int *largeArs[(1024 + 512 + 128)* 4];
            for (auto &j : largeArs) {
                int *largeAr = (int *) malloc(64 * 1024 * sizeof(int));
                for (int k=  0;k<64*1024;k++) {
                    largeAr[k] = k;
                }
                j = static_cast<int *>(largeAr);
            }
            for(auto &j: largeArs){
                free(j);
            }
//            printFunctionExecutionTime(read4objs);
//            printFunctionExecutionTime(getBytes);
//            printFunctionExecutionTime(getLineByLine);
        }
    }

    static void printFunctionExecutionTime(const std::function<void(void)> &functionPointer) {
        auto start = std::chrono::high_resolution_clock::now();
        functionPointer();
        auto finish = std::chrono::high_resolution_clock::now();
        auto duration = finish - start;
        long long int nanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        auto miliSeconds = nanoSeconds / 1000 / 1000;
        std::cout << miliSeconds << "ms\n";
    }
};


#endif //OPENGLSETUP_FILETHROUGHPUT_H
