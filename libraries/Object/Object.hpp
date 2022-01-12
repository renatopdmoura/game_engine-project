#ifndef OBJECT_H
#define OBJECT_H

#include "../Common/Common.hpp"
#include "../Math/Math.hpp"
#include "../SRW/SRW.hpp"

#include <glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Object : public SRW{
	private:
		struct vertexAttrib{
			vec3<float> position;
			vec3<float> normal;
			vec2<float> texcoord;
			vec3<float> tangent;
			vec3<float> bitangent;
			void show(){
				position.show("Position");
				normal.show("Normal");
				texcoord.show("TexCoord");
				tangent.show("Tangent");
				bitangent.show("Bitangent");
			}
		};
		uint shaderType;
		uint VAO;
		uint VBO;
		uint UBO;
		uint instanceMethod;
		std::string name;
		std::vector<float> vertex;
		std::vector<mat4<float>>* arrayModel;
		mat4<float> model;
	public:
		static std::vector<Object*> stack;
		static void completeness();

		// - Basic functions 
		Object(std::string model_path, std::string vs_path, std::string fs_path);
		Object(std::string model_path, uint type, std::vector<mat4<float>>* matrices = NULL, uint method = UNIFORM_ARRAY);
		~Object();
		void free();
		bool parser(std::string model_path);
		inline void search(std::vector<float>* container, std::ifstream* file, char code);
		void genBuffers();
		void instantiate(std::vector<mat4<float>>* matrices, uint method = UNIFORM_BLOCK);
		void updateInstances();

		// - Rendering
		void render();
		void render(uint instances);

		// - Getters
		std::string getName();
		mat4<float>& getModel();
		std::vector<float> getVertexBufferArray();
		uint getVAO() const;
		uint getVBO() const;
	};
#endif