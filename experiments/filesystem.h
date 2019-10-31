//
// Created by aidan on 30/10/2019.
//
#ifndef OPENGLSETUP_FILESYSTEM_H
#define OPENGLSETUP_FILESYSTEM_H

#include <string>

 #include "../../crefile.hpp"
#ifdef _MSC_VER
#include "../dependencies/crefile.hpp"
#endif

void navigate(){
    auto current = crefile::Path{"/mnt/c/Users/aidan/Documents"};
    while(true) {
        if(current.exists()) {
            if(current.extension().empty()) {
                try {
                    for (const auto &file : crefile::iter_dir(current)) {
                        std::cout << (file.is_directory() ? " 📁\t" : " 🗎\t");
                        std::cout << file.name() << std::endl;
                    }
                }
                catch (const std::exception e) {
                    std::cout << "error reading extensionless file system item as folder";
                }
            }
            else{
                std::cout << current.extension();
            }
        }

        std::string str;
        std::getline(std::cin, str);
        if(str=="exit"){
            return;
        }

        if((current/str).exists()) {
            current = current / str;
        }
    }
}

#endif //OPENGLSETUP_FILESYSTEM_H
