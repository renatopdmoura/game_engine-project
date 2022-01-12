#ifndef TERRAIN_H
#define TERRAIN_H

#include "../SRW/SRW.hpp"

#include <fstream>
#include <cassert>
#include <math.h>

class Terrain : public SRW{
public:
	Terrain(const char* path, unsigned char bitsPerPixel, unsigned int width, unsigned int height, float hs, float bs);
	~Terrain();
	void free();
	void loadHeightMap(const char* path, unsigned char bitsPerPixel, unsigned int width, unsigned int height);
	void getHeightAt(vec3<float>& position);
	void render();
	void generateIndexBuffer();
	void generateNormals();
	void generateVertexBuffers();
	inline int getFileLength(std::ifstream& file);
	inline float getHeightValue(const unsigned char* data, unsigned char numBytes);
private:
	uint VAO;
	uint VBO;
	uint IBO;
	std::vector<vec3<float>> posBuff;
	std::vector<vec3<float>> norBuff;
	std::vector<vec2<float>> uvsBuff;
	std::vector<uint> indices;
	uint heightmapWidth;
	uint heightmapHeight;
	float heightScale;
	float blockScale;
	mat4<float> model;
};
#endif