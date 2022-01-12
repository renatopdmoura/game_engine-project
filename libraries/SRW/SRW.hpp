#ifndef SRW_H
#define SRW_H
#include "../Common/Common.hpp"
#include "../libraries/Math/Math.hpp"
#include "../Datatype/Datatype.hpp"

#include <glew.h>
#include <SDL.h>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

class SRW{
private:
	struct uniformData{
		int location;
		std::string label;
	};
	struct uniform1b : public uniformData{
		uniform1b(std::string name, bool parm){
			value = parm;
			label = name;
		}
		bool value;
	};
	struct uniform1i : public uniformData{
		uniform1i(std::string name, uint parm, uint id = 0){
			value = parm;
			label = name;
			texID = id;
		}
		uint value;
		uint texID;
	};
	struct uniform1f : public uniformData{
		uniform1f(std::string name, float parm){
			value    = parm;
			label    = name;
			ptrValue = NULL;
		}
		uniform1f(std::string name, float* parm){
			ptrValue = parm;
			label    = name;
			value    = 0.0f;
		}
		float value;
		float* ptrValue;
	};
	struct uniform3f : public uniformData{
		uniform3f(std::string name, vec3<float> parm){
			value    = parm;
			label    = name;
			ptrValue = NULL;
		}
		uniform3f(std::string name, vec3<float>* parm){
			ptrValue = parm;
			label    = name;
		}
		vec3<float> value;
		vec3<float>* ptrValue;
	};
	struct uniformMat4f : public uniformData{
		uniformMat4f(std::string name, mat4<float>* parm){
			value = parm;
			label = name;
		}
		mat4<float>* value;
	};
	static void sharedUniforms(Sun* sun, std::vector<PointLight>* pointLightArray, std::vector<SpotLight>* spotLightArray);
public:	
	//Constructors
	SRW();
	virtual ~SRW();
		
	//To programs [DONED]
	static uint genProgramShader(std::string vs_path, std::string fs_path);
	static uint genProgramShader(std::string vs_path, std::string fs_path, std::string gs_path);
	static void genProgramShaders();

	//To buffers
	static void genGeometryBuffer(uint& gBuffer, uint& gDepth, uint& gPosition, uint& gNormal, uint& gAlbedo, uint width, uint height);
	static void sharedUniforms(mat4<float>& matrix, vec3<float>& cameraPos, Sun* sun, std::vector<PointLight>* pointLightArray, std::vector<SpotLight>* spotLightArray = NULL);
	static void sharedUniforms(mat4<float>& viewProj, vec3<float>& cameraPos, bool update = true);

	//Basic setters to init uniforms [DONED]
	void setUniform1b(std::string, bool value);
	void setUniform1f(std::string, float value);
	void setUniform1f(std::string name, float* value);
	void setUniform3f(std::string, vec3f value);
	void setUniform3f(std::string, vec3f* value);
	void setUniformMat4f(std::string name, mat4<float>* value);

	void addTexture(const char* texPath, const char* uniformName, uint texUnit, bool SRGB = false, bool SRGBA = false, uint* parmWidth = NULL, uint* parmHeight = NULL);
	void addTexture(uint& texID, const char* uniformName, uint texUnit, uint uniformType = 0);

	//Initializer uniforms
	void initUnifSampler2D();
	void initUnifSamplerCubemap();
	//Until here [DONE]
	void initUnif1b();
	void initUnif1f();
	void initUnif3f();
	void initUnifMat4f();
		
	//Define the shading program [DONED]
	void setProgram(uint& prog);
	uint& getProgram();

	//Getters arrays [DONED]
	uniform1b& getUniform1b(uint index);
	uniform1f& getUniform1f(uint index);
	uniform3f& getUniform3f(uint index);
	uniformMat4f& getUniformMat4f(uint index);
		
	//Search for uniform names in uniform arrays [DONED]
	int searchUniformSampler2D(std::string name);
	int searchUniform1b(std::string name);
	int searchUniform1f(std::string name);
	int searchUniform3f(std::string name);
	int searchUniformMat4f(std::string name);
	
	//Displays instance data on console [DONED]
	static uint programs[3];
	static std::vector<uint> gUBO;
	void info();
protected:
	void genShader(std::string vs_path, std::string fs_path);
	uint program;
	std::vector<uniform1i>     sampler2D;
	std::vector<uniform1i>     samplerCubemap;
	std::vector<uniform1b>	   unif1b;
	std::vector<uniform1f>     unif1f;
	std::vector<uniform3f>     unif3f;
	std::vector<uniformMat4f> unifMat4f;
};

#endif