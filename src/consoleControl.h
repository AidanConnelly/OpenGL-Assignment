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

struct makeInstancesJob
{
    int toMakeInstancesOf;
};

class ConsoleControl
{
    std::map<std::string, int> cache;
    std::vector<std::vector<MeshData>*> loaded;
    int numMeshDataWrtn;
    int numMeshDataRead;

    std::vector<makeInstancesJob> toMakeInstanceOf;
    int numMakeInstanceJobWrtn;
    int numMakeInstanceJobRead;

    std::vector<std::string> overrideTextureJobs;
    int numTextureOverridesWrtn;
    int numTextureOverridesRead;

    std::vector<int> exportMeshQueue;
    int exportMeshWrtn;
    int exportMeshRead;

    void printDirectoryStrucuture(std::filesystem::path current)
    {
        if (exists(current))
        {
            if (current.extension().empty())
            {
                try
                {
                    auto currentDirContents = std::filesystem::directory_iterator(current);
                    for (const auto& file : currentDirContents)
                    {
                        // std::string folderChar = "📁";
                        std::string folderChar = "o";
                        // std::string fileChar = "🗎";
                        std::string fileChar = "i";
                        std::cout << (std::filesystem::is_directory(file)
                                      ? " " + folderChar + "\t"
                                      : " " + fileChar + "\t");
                        std::cout << file.path().filename() << std::endl;
                    }
                }
                catch (const std::exception e)
                {
                    std::cout << "error reading extensionless file system item as folder";
                }
            }
            else
            {
                std::cout << current.extension();
            }
        }
    }

    void fuzzObj(){
        std::vector<char> contents;
        int size = 10*1024;
        std::random_device engine;
        for(int i =0;i<size;i++){
            char randChar = engine();
            contents.push_back(randChar);
        }
        objParser::parse(contents, "");
    }

    void loadMesh(std::filesystem::path current, std::string str)
    {
        std::string toLoad = str.substr(5, str.size() - 5);
        std::string fullFilePath = (current / toLoad).string();
        if (exists(current / toLoad))
        {
            std::vector<char> contents = fileThroughput::getBytes(fullFilePath);

            std::string extension = (current / toLoad).extension().string();

            std::vector<MeshData>* results = NULL;
            if (!cache.count(fullFilePath))
            {
                //todo
                if (extension == ".obj")
                {
                    std::vector<MeshData> toResults = objParser::parse(contents, current.string() + "\\");
                    results = new std::vector<MeshData>(std::move(toResults));
                }
                else if (extension == ".dae")
                {
                    std::vector<MeshData> toResults = daeParser::parse(contents, current.string() + "\\");
                    results = new std::vector<MeshData>(std::move(toResults));
                }
            }

            if (results)
            {
                loaded.push_back(results);
                cache[fullFilePath] = loaded.size() - 1;
            }
            int size = loaded.size();
            numMeshDataWrtn = size;

            //Add a mesh instance
            toMakeInstanceOf.push_back(makeInstancesJob{cache[fullFilePath]});
            int jobSize = toMakeInstanceOf.size();
            numMakeInstanceJobWrtn = jobSize;
        }
    }

    void overrideTexture(std::filesystem::path current, std::string str)
    {
        if (exists(current / str) && !is_directory(current / str))
        {
            overrideTextureJobs.push_back(absolute((current / str)).string());
            numTextureOverridesWrtn = overrideTextureJobs.size();
        }
    }

    void queueExportMesh()
    {
        exportMeshQueue.push_back(0);
        exportMeshWrtn++;
    }

public:

    void exportMesh(std::vector<MultiMesh*>& meshes, std::vector<MeshInstance>& instances)
    {
        if (exportMeshWrtn > exportMeshRead)
        {
            std::vector<char> buffer;
            int bitIndex = 0;
            encodeMultiMesh(meshes[0], 1.0/1000.0, buffer, bitIndex);
            MultiMesh* decoded = decodeMultiMesh(buffer);
            meshes.push_back( decoded);
            instances.push_back(MeshInstance(decoded));
            exportMeshRead++;
        }
    }

    void loopNavigation()
    {
        auto current = std::filesystem::path("C://");
        while (true)
        {
            printDirectoryStrucuture(current);

            std::string str;
            std::getline(std::cin, str);
            std::string overrideTextureCmdPrefix = "override texture ";
            if (str == "exit")
            {
                return;
            }
            else if (str == "..")
            {
                current = current.parent_path();
            }
            else if (str.rfind("load ", 0) == 0)
            {
                loadMesh(current, str);
            }
            else if (exists(current / str) && is_directory(current / str))
            {
                current = current / str;
            }
            else if (str.rfind(overrideTextureCmdPrefix, 0) == 0)
            {
                overrideTexture(current, str.substr(overrideTextureCmdPrefix.size(), str.size() - 5));
            }
            else if (str == "export")
            {
                queueExportMesh();
            }
            else if (str == "fuzz obj"){
                fuzzObj();
            }
        }
    }

    void loadMeshesInto(std::vector<MultiMesh*>& meshes, std::vector<MeshInstance>& meshInstances)
    {
        for (int i = numMeshDataRead; i < numMeshDataWrtn; i++)
        {
            std::vector<Mesh*> toMakeMultiMeshOf;
            for (auto& a : *loaded[i])
            {
                a.BindTextures();
                Mesh* meshPtr = new Mesh(a);
                toMakeMultiMeshOf.push_back(meshPtr);
                numMeshDataRead = i + 1;
            }
            MultiMesh* multiMesh = new MultiMesh(toMakeMultiMeshOf);
            meshes.push_back(multiMesh);
        }

        for (int i = numMakeInstanceJobRead; i < numMakeInstanceJobWrtn; i++)
        {
            int makeIndexOf = toMakeInstanceOf[i].toMakeInstancesOf;
            MultiMesh* mesh = meshes[makeIndexOf];
            MeshInstance instance = MeshInstance(mesh);
            meshInstances.push_back(instance);
            numMakeInstanceJobRead = i + 1;
        }
    }

    void loadOverrideTextures(std::vector<Texture>& toLoadInto)
    {
        for (int i = numTextureOverridesRead; i < numTextureOverridesWrtn; i++)
        {
            toLoadInto.push_back(Texture(overrideTextureJobs[i]));
            numTextureOverridesRead = i + 1;
        }
    }
};

#endif //OPENGLSETUP_CONSOLECONTROL_H
