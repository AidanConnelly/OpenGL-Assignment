//
// Created by m on 23/11/2019.
//

#ifndef OPENGLSETUP_CONSOLECONTROL_H
#define OPENGLSETUP_CONSOLECONTROL_H

//
// Created by m on 23/11/2019.
//

#include "src/dotFuzFormat.h"
#include "src/GMM.h"
#include "src/HuffmanCoding.h"
#include <thread>
#include <filesystem>
#include "vsSolution/vsSolution/objParser.h"
#include <iostream>
#include "src/Mesh.h"
#include "src/Texture.h"
#include "experiments/filesystem.h"
#include "experiments/preliminary/fileThroughput/fileThroughput.h"
#include "shaderProgram.h"
#include "shader.h"
#include "GLFW/glfw3.h"
#include "GL/freeglut.h"
#include "GL/glew.h"
#include "consoleControl.h"
#include <random>

struct makeInstancesJob {
    int toMakeInstancesOf;
};

struct exportMeshJob {
    int meshToExport;
    std::string pathToExportTo;
};

class ConsoleControl {
    std::map<std::string, int> cache;
    std::vector<std::vector<MeshData> *> loaded;
    int numMeshDataWrtn;
    int numMeshDataRead;

    std::vector<makeInstancesJob> toMakeInstanceOf;
    int numMakeInstanceJobWrtn;
    int numMakeInstanceJobRead;

    std::vector<std::string> overrideTextureJobs;
    int numTextureOverridesWrtn;
    int numTextureOverridesRead;

    std::vector<exportMeshJob> exportMeshQueue;
    int exportMeshWrtn;
    int exportMeshRead;

    std::vector<int> removeMeshQueue;
    int removeMeshWrtn;
    int removeMeshRead;

    bool phong = false;
    std::random_device engine;

    void listMeshes() {
        std::cout << std::endl << "meshes: " << std::endl;
        for (auto &pair: cache) {
            std::cout << pair.second << ") \t" << pair.first << std::endl;
        }
    }

    void printDirectoryStrucuture(filesystem::path current) {
        if (exists(current)) {
            if (current.extension().empty()) {
                try {
                    auto currentDirContents = filesystem::directory_iterator(current);
                    for (const auto &file : currentDirContents) {
                        // std::string folderChar = "📁";
                        std::string folderChar = "o";
                        // std::string fileChar = "🗎";
                        std::string fileChar = "i";
                        std::cout << (filesystem::is_directory(file)
                                      ? " " + folderChar + "\t"
                                      : " " + fileChar + "\t");
                        std::cout << file.path().filename() << std::endl;
                    }
                }
                catch (const std::exception e) {
                    std::cout << "error reading extensionless file system item as folder";
                }
            } else {
                std::cout << current.extension();
            }
        }
    }

    void fuzz(filesystem::path current, std::string str) {
        for (int i = 0; i < 100; i++) {
            std::string fullFilePath = (current / str).string();
            if (exists(current / str)) {
                std::vector<char> contents = fileThroughput::getBytes(fullFilePath);

                float logLowerCorruptionRate = log(1.0 / ((float) contents.size()));
                float logUpperCorruptionRate = 0;
                float logCorruptionRate =
                        (logUpperCorruptionRate - logLowerCorruptionRate) * ((float) rand() / (float) (RAND_MAX)) +
                        logLowerCorruptionRate;
                float corruptionRate = exp(logCorruptionRate);
                std::cout << std::endl << "fuzzing corruption rate: " << corruptionRate << std::endl;
                for (auto &byte: contents) {
                    if ((float) rand() / (float) (RAND_MAX) < corruptionRate) {
                        byte = engine();
                    }
                }
                loadFromContents(current, str, contents);
            }
        }
        std::cout << std::endl << "finished fuzzing" << std::endl;
    }

    void loadMesh(filesystem::path current, std::string str) {
        std::string toLoad = str.substr(5, str.size() - 5);
        std::string fullFilePath = (current / toLoad).string();
        if (exists(current / toLoad)) {
            std::vector<char> contents = fileThroughput::getBytes(fullFilePath);

            if (!cache.count(fullFilePath)) {
                loadFromContents(current, toLoad, contents);
            } else {
                makeInstanceOfPath(fullFilePath);
            }
        }
    }

    void loadFromContents(const filesystem::path &current, const std::string &str, std::vector<char> &contents) {
        std::string fullFilePath = (current / str).string();
        std::string extension = (current / str).extension().string();

        std::vector<MeshData> *results = NULL;
        //todo
        if (extension == ".obj") {
            try {
                std::vector<MeshData> toResults = objParser::parse(contents, current.string() + "\\");
                results = new std::vector<MeshData>(std::move(toResults));
            }
            catch (...) {
                std::cout << std::endl << "Couldn't load OBJ" << std::endl;
            }
        } else if (extension == ".dae") {
            try {
                std::vector<MeshData> toResults = daeParser::parse(contents, current.string() + "\\");
                results = new std::vector<MeshData>(std::move(toResults));
            }
            catch (...) {
                std::cout << std::endl << "Couldn't load DAE" << std::endl;
            }
        } else if (extension == ".fuz") {
            std::vector<MeshData> toResults;
            MeshData meshData = decodeMultiMesh(contents);
            toResults.push_back(meshData);
            results = new std::vector<MeshData>(std::move(toResults));
        }

        if (results) {
            loaded.push_back(results);
            cache[fullFilePath] = loaded.size() - 1;

            int size = loaded.size();
            numMeshDataWrtn = size;
            makeInstanceOfPath(fullFilePath);

        }
    }

    void makeInstanceOfPath(const std::string &fullFilePath) {//Add a mesh instance
        toMakeInstanceOf.push_back(makeInstancesJob{cache[fullFilePath]});
        int jobSize = toMakeInstanceOf.size();
        numMakeInstanceJobWrtn = jobSize;
    }

    void overrideTexture(filesystem::path current, std::string str) {
        if (exists(current / str) && !is_directory(current / str)) {
            overrideTextureJobs.push_back(absolute((current / str)).string());
            numTextureOverridesWrtn = overrideTextureJobs.size();
        }
    }

    void queueExportMesh(filesystem::path current) {
        listMeshes();
        std::cout << std::endl << "number mesh to export?" << std::endl;
        std::string str;
        std::getline(std::cin, str);
        int toExport = std::stoi(str, nullptr);
        std::cout << std::endl << "enter file name: (???.fuz)" << std::endl;
        std::string fileName;
        std::getline(std::cin, fileName);
        fileName += ".fuz";
        fileName = (current / fileName).string();
        exportMeshQueue.push_back(exportMeshJob{toExport, fileName});
        exportMeshWrtn++;
    }

public:

    void exportMesh(std::vector<MultiMesh *> &meshes, std::vector<MeshInstance> &instances) {
        if (exportMeshWrtn > exportMeshRead) {
            std::vector<char> buffer;
            int bitIndex = 0;
            exportMeshJob job = exportMeshQueue[exportMeshRead];
            encodeMultiMesh(meshes[job.meshToExport], 1.0 / 1000.0, buffer, bitIndex);

            std::string fileName = job.pathToExportTo;
            std::ofstream fout(fileName, std::ios::out | std::ios::binary);
            fout.write(&buffer[0], buffer.size() * sizeof(char));
            fout.close();

            exportMeshRead++;
        }
    }

    void removeMesh(std::vector<MultiMesh*> &meshes, std::vector<MeshInstance> &instances){
        if(removeMeshWrtn>removeMeshRead){
            int toRemove = removeMeshQueue[removeMeshRead];
            auto multiMeshPtr = meshes[toRemove];
            for(MeshInstance& instance:instances){
                if(instance.instanceOf == multiMeshPtr){
                    instances.erase(instances.begin()+(&instance-&instances[0]));
                }
            }
            for(auto& pair:cache){
                if(pair.second == toRemove){
                    cache.erase(cache.find(pair.first));
                }
            }
            meshes[toRemove] = nullptr;
            delete multiMeshPtr;
        }
    }

    void loopNavigation() {
        auto current = filesystem::path("C://");
        while (true) {
            printDirectoryStrucuture(current);

            std::string str;
            std::getline(std::cin, str);
            std::string overrideTextureCmdPrefix = "override texture ";
            std::string fuzzPrefix = "fuzz ";
            if (str == "exit") {
                return;
            } else if (str == "..") {
                current = current.parent_path();
            } else if (str.rfind("load ", 0) == 0) {
                loadMesh(current, str);
            } else if (exists(current / str) && is_directory(current / str)) {
                current = current / str;
            } else if (str.rfind(overrideTextureCmdPrefix, 0) == 0) {
                overrideTexture(current, str.substr(overrideTextureCmdPrefix.size(),
                                                    str.size() - overrideTextureCmdPrefix.size()));
            } else if (str == "export") {
                queueExportMesh(current);
            } else if (str.rfind(fuzzPrefix, 0) == 0) {
                fuzz(current, str.substr(fuzzPrefix.size(), str.size() - fuzzPrefix.size()));
            } else if (str == "toggle phong") {
                phong = true;
            } else if (str == "list meshes") {
                listMeshes();
            } else if (str == "shadows on") {
                shadows = true;
            } else if (str == "shadows off"){
                shadows = false;
            } else if (str == "remove file"){
                listMeshes();
                std::cout << std::endl << "number mesh to remove?" << std::endl;
                std::string str;
                std::getline(std::cin, str);
                int toExport = std::stoi(str, nullptr);
                removeMeshQueue.push_back(toExport);
                removeMeshWrtn = removeMeshQueue.size();
            }
        }
    }

    void loadMeshesInto(std::vector<MultiMesh *> &meshes, std::vector<MeshInstance> &meshInstances) {
        for (int i = numMeshDataRead; i < numMeshDataWrtn; i++) {
            std::vector<Mesh *> toMakeMultiMeshOf;
            std::vector<MeshData> *ptrToMeshDataVector = loaded[i];
            for (MeshData &a : *ptrToMeshDataVector) {
                a.BindTextures();

                //deleted when multimesh deleted
                Mesh *meshPtr = new Mesh(a);
                toMakeMultiMeshOf.push_back(meshPtr);
                numMeshDataRead = i + 1;
            }
            delete ptrToMeshDataVector;

            //deleted when mesh removed from cache
            MultiMesh *multiMesh = new MultiMesh(toMakeMultiMeshOf);
            meshes.push_back(multiMesh);
        }

        for (int i = numMakeInstanceJobRead; i < numMakeInstanceJobWrtn; i++) {
            int makeIndexOf = toMakeInstanceOf[i].toMakeInstancesOf;
            MultiMesh *mesh = meshes[makeIndexOf];
            MeshInstance instance = MeshInstance(mesh);
            meshInstances.push_back(instance);
            numMakeInstanceJobRead = i + 1;
        }
    }

    void loadOverrideTextures(std::vector<Texture> &toLoadInto) {
        for (int i = numTextureOverridesRead; i < numTextureOverridesWrtn; i++) {
            toLoadInto.push_back(Texture(overrideTextureJobs[i]));
            numTextureOverridesRead = i + 1;
        }
    }

    bool getPhong() {
        return phong;
    }

    bool shadows = false;
};

#endif //OPENGLSETUP_CONSOLECONTROL_H
