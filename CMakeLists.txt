cmake_minimum_required(VERSION 3.14)
project(soft356a3)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -O0)
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
        src/daeParsing
        src/daeParsing/bufferParseResult.h
        src/daeParsing/daeParser.cpp
        src/daeParsing/daeParser.h
        src/daeParsing/DoParallel.h
        src/daeParsing/stringToFloatFast.cpp
        src/daeParsing/stringToFloatFast.h
        src/daeParsing/typedefs.h
        src/daeParsing/xmlNode.h
        src/daeParsing/XMLParseState.h
        src/daeParsing/xmlParsingStackMember.h
        src/dotFuz
        src/dotFuz/bitBuffer.cpp
        src/dotFuz/bitBuffer.h
        src/dotFuz/dotFuzFormat.cpp
        src/dotFuz/dotFuzFormat.h
        src/dotFuz/GMM.h
        src/dotFuz/HuffmanCoding.h
        src/files
        src/files/fileReader.h
        src/files/fs.h
        src/graphics
        src/graphics/Mesh.cpp
        src/graphics/Mesh.h
        src/graphics/Texture.cpp
        src/graphics/Texture.h
        src/graphics/Vertex.cpp
        src/graphics/Vertex.h
        src/objParsing
        src/objParsing/objParser.cpp
        src/objParsing/objParser.h
        src/shaders
        src/shaders/shader.h
        src/shaders/shaderProgram.h
        src/shaders/shaderType.cpp
        src/shaders/shaderType.h
        src/consoleControl.h
        src/safeIndex.h
        main.cpp
        stb_image.h)

target_link_libraries(soft356a3 :freeglut.lib :glew32.lib :glfw3dll.lib opengl32 glu32 gdi32)
