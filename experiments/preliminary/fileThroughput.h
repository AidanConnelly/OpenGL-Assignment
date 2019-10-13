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

    static void getBytes() {
        for (int i = 0; i < 1; i++) {

            std::ifstream myfile("D:\\reasonableSize.obj");

            int timesRead = 0;

            //512 KB chunks for fast opening of smaller files
            int maxNChars = 512 * 1024;
            char buffer[maxNChars]; // create a buffer
            while (!myfile.eof()) {
                timesRead++;
                myfile.read(buffer, maxNChars); // read to buffer
            }
            std::cout << timesRead << std::endl;
            myfile.close();
        }

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
            printFunctionExecutionTime(getBytes);
            printFunctionExecutionTime(getLineByLine);
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
