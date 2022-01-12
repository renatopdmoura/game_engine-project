Pura is a set of classes specialized in graphic rendering based on the rasterization engine.

The core technologies are:
			SDL	OpenGL	FreeType

So far, there are classes to provide the most basic features of a renderer, such as:

Shader Resources Wrapper(SRW): intended to manage data from a shader program.

Object: intended for loading three-dimensional models by interpreting .obj files

Terrain Loader: designed to build terrains based on height maps.

Camera: intended to use a three-dimensional control camera of the "fly" type.

Text: intended for rendering texts.

Audio: intended to reproduce sounds.

Functions and other relevant classes

Math: usual mathematical functions in computer graphics.

Common: header for global data.

Core: Initialization of the workbench.

Datatype: variables formatted in blocks (classes) for greater efficiency of use.

Quad: class for debugging purposes that works as a rendering framework, mostly intended for drawing surface for framebuffers.

CommandLine: class to control runtime rendering parameters via text input.

External headers:

stbi: specialized in image loading.