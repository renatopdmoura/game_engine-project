#ifndef COMMON_H
#define COMMON_H
#include <SDL.h>
#include <glew.h>
#include <string>

typedef unsigned int uint;
typedef unsigned char uchar;

extern SDL_Window* ext_window;
extern int ext_screen_width;
extern int ext_screen_height;

extern bool QUIT;

void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);
std::string readShaderSource(const char* pathfile);
bool createProgram(GLuint& program);
bool createShader(GLuint& program, GLenum shaderType, const std::string& shaderSource);
bool linkProgram(GLuint& program);

enum{
	UNIFORM_COLOR,
	TEXTURIZED,
	UNIFORM_ARRAY,
	UNIFORM_BLOCK,
	SRGB_PROFILE,
	SRGBA_PROFILE,
	DEFAULT_PROFILE
};
#endif