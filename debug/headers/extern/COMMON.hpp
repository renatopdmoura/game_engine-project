#ifndef COMMON_H
#define COMMON_H

#ifndef DEBUG
#define DEBUG 0
#endif

#include <glew.h>
#include <SDL.h>
#include "../math/vec3.hpp"
#include "../math/vec4.hpp"
#include "../math/mat2.hpp"
#include "../math/mat4.hpp"

//program shaders
std::string readShaderSource(const char* pathfile);
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);
bool createProgram(GLuint& program);
bool createShader(GLuint& program, GLenum shaderType, const std::string& shaderSource);
bool linkProgram(GLuint& program);

//buffer
void mapBuffer(unsigned int index, size_t buffer_size);

//math
extern mat4<float> perspective(float angle, int w, int h);

//conversion
float radians(const float& angle);

//fixation
float clamp(const float& v, const float& min, const float& max);

//comparation
bool equalf(vec3f& A, vec3f& B);
bool notequalf(vec3f& A, vec3f& B);

//geometry
vec3f normalize(const vec3f& vector);
vec3f cross(const vec3f& a, const vec3f& b);
float dot(const vec3f& a, const vec3f& b);
float dot(const vec2f& a, const vec2f& b);

//2D operation
mat2<float> rotateZ_2D(const float& roll);

//3D operation
mat4<float> translate(const vec3f& vector);
mat4<float> scale(const vec3f& vector);
mat4<float> rotateX(const float& pitch);
mat4<float> rotateY(const float& yaw);
mat4<float> rotateZ(const float& roll);
mat4<float> orientation(mat4<float> mat);

//view
mat4<float> lookAtRH(const vec3f& eye, const vec3f& target, const vec3f up = vec3f(0.0f, 1.0f, 0.0f));

//projective
mat4<float> perspective(float fovy, float aspect, float zNear, float zFar);
mat4<float> orthographic(float r, float l, float t, float b, float near, float far);
mat4<float> ortho(float r, float l, float t, float b);

typedef unsigned int uint;
typedef unsigned char uchar; 


#endif