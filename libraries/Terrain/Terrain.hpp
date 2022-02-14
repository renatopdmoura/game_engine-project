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
	void setBaseTex(std::string pathAlbedo, std::string pathNormal, std::string pathRough, std::string pathAO){
		std::string root = "../assets/textures/terrain/";
		addTexture(root + pathAlbedo, "material.albedo[0]", 0);
		addTexture(root + pathNormal, "material.normal[0]", 2);
		addTexture(root + pathRough, "material.roughness[0]", 4);
		addTexture(root + pathAO, "material.ao[0]", 6);
	}
	void setSlopeTex(std::string pathAlbedo, std::string pathNormal, std::string pathRough, std::string pathAO){
		std::string root = "../assets/textures/terrain/";
		addTexture(root + pathAlbedo, "material.albedo[1]", 1);
		addTexture(root + pathNormal, "material.normal[1]", 3);
		addTexture(root + pathRough, "material.roughness[1]", 5);
		addTexture(root + pathAO, "material.ao[1]", 7);
	}
	void setPathTex(std::string pathMask, std::string pathAlbedo, std::string pathNormal, std::string pathRough, std::string pathAO){
		std::string root = "../assets/textures/terrain/";
		addTexture("../assets/textures/terrain/paths/" + pathMask, "material.pathMask", 8);
		addTexture(root + pathAlbedo, "material.albedo[2]", 9);
		addTexture(root + pathNormal, "material.normal[2]", 10);
		addTexture(root + pathRough, "material.roughness[2]", 11);
		addTexture(root + pathAO, "material.ao[2]", 12);
	}
	#if RENDER_DEBUG_MODE
		void renderDebugDepth(mat4<float>& viewProj, float zNear, float zFar);
		void renderDebugNormal(mat4<float>& view, mat4<float>& projection);
	#endif
		mat4<float>& getModel(){
			return model;
		}
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