To complement the development, you should write a brief introduction and description in markdown format
which details the project aims, used software, used libraries, setup, third party assets and how the code fits
together to be used in other cases. You will use this during the demo session to walk us through your project.

A brief write-up in markdown format, starting with your aim and intended as a project
documentation:
 Which version Visual Studio, OpenGL/Vulkan
 How do I work it?
 How does your code fit together and how should a programmer navigate it ( not
intended to be an exercise in formally documenting program code)
 Anything else which will help us to understand how your prototype works.
 If you went for the advanced loader, include a small argument for your own file format
and an evaluation of the loader and the different file format (memory, loading times,
etc.)

The report is
concise and
explains setup and
structure of the
program

Arguments and
Evaluation of the
export file format
are sound.

### Introduction ###

This model loader can load COLLADA and .OBJ files, export and load to a compressed file format, move the models in 3D space, switch between PHONG spot lighting and lighting with shadows, (using a special shadow map filtering technique). It uses CPU-parallelism to speed up loading of COLLADA & OBJ files.

### Description ###

This model loader has been developed with file I/O & parsing performance as a priority. This has causes the use of a finite state machine parsing first stage, then possibly followed by a second map-reduce-esque stage, where mapping and filtering of elements can be parallelized.

### Project aims ###

Goals:

  ☑ Import OBJ files

  ☑ Import DAE files

  ☑ Export to another format

  ☑ Load textures from models

  ☑ Override textures on models with another

  ☑ Implement PHONG lighting

  ☑ Implement shadow mapping

  ☑ Instances of models can be removed

  ☑ The data of a models can be deleted and the associated memory freed

  ☑ Safely fail fuzzed OBJ files

  ☑ Safely fail fuzzed DAE files

  ☑ Free all memory promptly so as not to cause memory leaks

  ☑ Import from the format exported to


### Used software ###

The project was developed mainly in CLion with CMake, due to the authors familiarity with phpStorm, IDEA, pyCharm & RIDER, much of development was also in VS19, however not using the Cmake.

CLion was mainly used for:
 * heavy refactoring/programming, easy due to the tools provided in jetbrains IDEs
 * testing WSL & MinGW support
 * using valgrind

 VS19 has to be used for:
 * Heap profiling
 * Performance analyizing, which is essential for finding bottlenecks such as unnecessary memory copying between function calls
 * Lab computer support

Valgrind helped diagnose various problems including:
 * 2 cases of heap corruption
 * 1 case of overrite bounds of a span of memory

gDebugger is an incredibly useful tool, which I've used in the development of various parts, most importantly discovering nvidia disallows a colour depth of > 10 bits on its consumer GPUs

### Used libraries ###

The only used libraries are GLFW, GLEW, GLUT, GLM and STB.

### Setup ###

Clone the project and open the VS solution, add the glm include dir to the project's include dirs and it should compile and run. You may need to tweek the "prefix" parameter if the shaders arn't relative to the working dir.

### Source layout ###

#### src folder ####

 * bufferParseResult: a struct containing the results of the operation which parses the float and index buffers.
 * daeParser: contains most the logic for parsing the bytes into xml and the xml into a model, relatively messy due to the large number of edge cases in the COLLADA specification.
 * DoParallel: simply executes functions generated by a lambda from an int in parallel.
 * stringToFloatFast: a custom float parser. According to all resources online on float parsing, writing a native routine to consume the characters one by one is fastest, unless the floats are garunteed to be a specific format, then SIMD can be used (this was looked into but proved to be impossible).
 * typeDefs: simply shortcuts for certain types, e.g. std::vector<xmlNode>.
 * xmlNode: lightweight struct used in both the parsing of the xml and the stages therafter.
 * XMLParseState: enum of 6 states of the FSM xml parser.
 * xmlParsingStackMember: the stack of the FSM xml parser is formed of these.

### Commands and controls ###

### Dot fuz ###

### Shadow mapping technique ###
