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

### Used libraries ###

### Setup ###

### Source layout ###

### Commands and controls ###

### Dot fuz ###

### Shadow mapping technique ###
