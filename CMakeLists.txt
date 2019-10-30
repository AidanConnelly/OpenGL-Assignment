cmake_minimum_required(VERSION 3.7)
project(OpenGLSetup)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O3")
set(SOURCE_FILES main.cpp GLAD/src/glad.c)
set(HEADER_FILES
        shader.h
        shaderProgram.h
        experiments/preliminary/fileThroughput/fileThroughput.cpp
        experiments/preliminary/fileThroughput/fileThroughput.h
        experiments/preliminary/prototypeParsers/daeParser.cpp
        experiments/preliminary/prototypeParsers/daeParser.h
        experiments/preliminary/prototypeParsers/prototypeObjParser.cpp
        experiments/preliminary/prototypeParsers/prototypeObjParser.h
        experiments/preliminary/prototypeParsers/stringToFloatFast.cpp
        experiments/preliminary/prototypeParsers/stringToFloatFast.h
        experiments/preliminary/prototypeParsers/XMLParseState.cpp
        experiments/preliminary/prototypeParsers/XMLParseState.h
        experiments/preliminary/prototypeParsers/xmlParsingStackMember.cpp
        experiments/preliminary/prototypeParsers/xmlParsingStackMember.h
        experiments/preliminary/prototypeParsers/typedefs.cpp
        experiments/preliminary/prototypeParsers/typedefs.h
        experiments/filesystem.h src/Mesh.cpp src/Mesh.h src/Texture.cpp src/Texture.h)

set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/build)

include_directories(include)

#Include library files
include_directories(GLAD/include)
include_directories(glm/include)

file(COPY resources DESTINATION ${dir}/build)
file(COPY shaders DESTINATION ${dir}/build)


#Add deps
add_subdirectory(dependencies)

add_executable(OpenGLSetup ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(OpenGLSetup glfw)
