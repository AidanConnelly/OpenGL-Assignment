cmake_minimum_required(VERSION 3.14)
project(soft356a3)

set(CMAKE_CXX_STANDARD 17)
include_directories(.)
include_directories(experiments)
include_directories(experiments/preliminary/fileThroughput)
include_directories(experiments/preliminary/prototypeParsers)
include_directories(src)
include_directories(vsSolution/vsSolution)
include_directories(vsSolution/vsSolution)

include_directories(vsSolution/packages/nupengl.core.0.1.0.1/build/native/include)
include_directories(glm/include)

#link_directories(vsSolution/packages/nupengl.core.0.1.0.1/build/native/lib/x64)
link_directories(vsSolution/packages/nupengl.core.0.1.0.1/build/native/lib/Win32)
link_directories(vsSolution/packages/nupengl.core.redist.0.1.0.1/build/native/bin/Win32)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(soft356a3
        experiments/preliminary/fileThroughput/fileThroughput.cpp
        experiments/preliminary/fileThroughput/fileThroughput.h
        experiments/preliminary/prototypeParsers/bufferParseResult.cpp
        experiments/preliminary/prototypeParsers/bufferParseResult.h
        experiments/preliminary/prototypeParsers/daeParser.cpp
        experiments/preliminary/prototypeParsers/daeParser.h
        experiments/preliminary/prototypeParsers/prototypeObjParser.cpp
        experiments/preliminary/prototypeParsers/prototypeObjParser.h
        experiments/preliminary/prototypeParsers/stringToFloatFast.cpp
        experiments/preliminary/prototypeParsers/stringToFloatFast.h
        experiments/preliminary/prototypeParsers/typedefs.cpp
        experiments/preliminary/prototypeParsers/typedefs.h
        experiments/preliminary/prototypeParsers/xmlNode.cpp
        experiments/preliminary/prototypeParsers/xmlNode.h
        experiments/preliminary/prototypeParsers/XMLParseState.cpp
        experiments/preliminary/prototypeParsers/XMLParseState.h
        experiments/preliminary/prototypeParsers/xmlParsingStackMember.cpp
        experiments/preliminary/prototypeParsers/xmlParsingStackMember.h
        experiments/filesystem.h
        src/bitBuffer.h
        src/dotFuzFormat.cpp
        src/dotFuzFormat.h
        src/GMM.h
        src/HuffmanCoding.h
        src/Mesh.cpp
        src/Mesh.h
        src/Texture.cpp
        src/Texture.h
        src/Vertex.cpp
        src/Vertex.h
        vsSolution/vsSolution/bitBuffer.cpp
        vsSolution/vsSolution/DoParallel.h
        vsSolution/vsSolution/objParser.cpp
        vsSolution/vsSolution/objParser.h
        main.cpp
        shader.h
        shaderProgram.h
        shaderType.cpp
        shaderType.h
        stb_image.h)

target_link_libraries(soft356a3 :freeglut.lib :glew32.lib :glfw3dll.lib opengl32 glu32 gdi32)
