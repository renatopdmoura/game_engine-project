#ifndef TERRAIN_H
#define TERRAIN_H

#ifndef DEBUG_VARIABLES_HEIGHTMAP
#define DEBUG_VARIABLES_HEIGHTMAP 0
#endif

#include <SDL.h>
#include <math.h>
#include <fstream>
#include <cassert>
#include <random>

#include "../math/vec3.hpp"
#include "../world/Scene.hpp"
#include "../SRW.hpp"

using namespace srw;

class Terrain : public SRW{
public:
	Terrain(float height_scale, float block_scale, Scene& parm);
	~Terrain();
	void fx(uint width = 256, uint height = 256){
		std::vector<vec3f> posbuff;
		std::vector<vec2f> texbuff;
		posbuff.resize(width * height);
		texbuff.resize(width * height);

		std::default_random_engine gen;
		std::uniform_real_distribution<float> dis(0.0f, 5.0f);

		float m = 0.0f;
		float U = 0.0f;
		float a = 2.0f;
		float x = 0.0f;
		float b = 1.0f;

		for(uint j = 0; j < height; ++j){
			for(uint i = 0; i < width; ++i){
				float S = (i / float(width - 1)) - 0.5f;
				if(S < m){
					U = b;
				}
				else if(S > m && S < m + 0.25f){
					U = pow(a, S * 10.0f);
				}
				float T = j / float(height - 1);
				uint index = (j * width) + i;
				posbuff[index] = vec3f((S * (width - 1)), U * 10.0f, (T * (height - 1)) - (height * 0.5f));
				texbuff[index] = vec2f(S, T);
			}
		}
		positionBuffer.resize(width * height);
		normalBuffer.resize(width * height);
		texcoordBuffer.resize(width * height);
		positionBuffer = posbuff;
		texcoordBuffer = texbuff;
		generateIndexBuffer();
		generateNormals();
		generateVertexBuffers();
	}
	void free();
	void getHeightAt(vec3f& position);
	void loadHeightMap(const char* path, unsigned char bitsPerPixel, unsigned int width, unsigned int height);
	void render();
	void renderFromDirectionalLightView(uint& programID, mat4<float>& view, mat4<float>& model);
	void renderFromPointLightView(uint& programID, std::vector<mat4<float>>& transforms, mat4<float>& model, vec3f& light_pos, float zFar);
	void renderMinimap(uint& programID, mat4<float>& view, mat4<float>& model);
	void generateIndexBuffer();
	void generateNormals();
	void generateVertexBuffers();
	//Utility
	inline int getFileLength(std::ifstream& file);
	inline float getHeightValue(const unsigned char* data, unsigned char numBytes);
	//Debug
	void keyboardEvent(SDL_Event* event);
	void memory_usage();
private:
	uint VAO;
	uint VBO;
	uint IBO;
	std::vector<vec3f> positionBuffer;
	std::vector<vec3f> normalBuffer;
	std::vector<vec2f> texcoordBuffer;
	std::vector<uint> indexBuffer;

	uint heightmapWidth;
	uint heightmapHeight;
	float heightScale;
	float blockScale;

#if DEBUG_VARIABLES_HEIGHTMAP
	float fTexcoord = 1024.0f; 
	float fWeight   = 0.1f;
	bool WEIGHT     = false;
	bool TEXCOORD   = false;
#endif
};

#endif