#ifndef OBJECT_H
#define OBJECT_H

#include "../Common/Common.hpp"
#include "../SRW/SRW.hpp"
#include "../Camera/Camera.hpp"
// #include "../Text/Text.hpp"

// - Glew/OpenGL
#include "GL/glew.h"

// - Standart header files
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

// - Assimp header files
#include "Assimp/Importer.hpp"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"

class Object : public SRW{
	public:
		static std::vector<Object*> stack;
		static void renameIfExists();

		// - Physics
		static float forceG;
		void AABBxSphere(Camera& camera);
		void AABBxAABB(Camera& camera); 
		void SpherexSphere(Camera& camera);

		static void completeness();
		#if RENDER_DEBUG_MODE
			// static void select(Camera& camera, SDL_Event* event, std::vector<Text>& objList);
			static void renderDebugDepth(mat4<float>& viewProj, float zNear, float zFar);
			static void renderDebugNormal(mat4<float>& view, mat4<float>& projection);
		#endif

		// - Initialize and end 
		Object(std::string model_path, std::string vs_path, std::string fs_path);
		Object(std::string model_path, uint type, std::vector<mat4<float>>* matrices = NULL, uint method = UNIFORM_ARRAY);
		~Object();
		void free();
		bool parser(std::string model_path);
		inline void search(std::vector<float>* container, std::ifstream* file, char code);
		void genBuffers();

		// - Instances rendering
		void instantiate(std::vector<mat4<float>>* matrices, uint method = UNIFORM_BLOCK);
		void updateInstances();
	
		// - Tais métodos impossibilita o rearanjo das multiplicações entre matrizes
		void pushMatrix(mat4<float>);
		void updateModelMatrixPerFrame(mat4<float> matrix);

		// - Rendering
		void render();
		void render(uint instances, uint frame);
		void render(uint frame, bool repeat, uint msBegin, uint msEnd);

		// - Setters
		void setName(std::string pname);	
		void setMaterial(vec3<float> albedo, float roughness, float ao, float matellic);
		void setMaterial(Material& material);
		void setTextures(std::string albedo, std::string normal, std::string roughness, std::string ao, float metallic);

		// - Getters		
		uint getVAO() const;
		uint getVBO() const;
		uint getUBO() const;
		uint getShaderType() const;
		std::string getName() const;
		std::vector<float> getVertexBuffer();
		mat4<float>& getModel();

		uint getNextTick() const{
			return next_tick;
		}

	private:
		struct vertexAttrib{
			vec3<float> position;
			vec3<float> normal;
			vec2<float> texcoord;
			vec3<float> tangent;
			vec3<float> bitangent;
		};
		uint VAO;
		uint VBO;
		uint UBO;
		uint shaderType;
		uint instanceMethod;
		std::string name;
		std::vector<float> vertex;
		mat4<float> model;
		std::vector<mat4<float>>* arrayModel;

		// - Membros voltados as funcionalidades de animação
		std::vector<std::vector<float>> keyframeBuffer;
		bool enable_multiple_files = false;
		bool rewrite_buffer_once = true;
		uint next_tick = 0;

		// - Posições relativas(objeto e observador) anteriores a uma detecção de colisão
		vec3<float> cameraBeforeCollision;
		mat4<float> modelBeforeCollision;
		
		// - Usar uma pilha de matrizes requer poder determinar a ordem de operações de escala, rotação e translação
		std::vector<mat4<float>> transforms;
	};

	// - New way to load 3d models using Assimp
	// - Algorithm explanation(pt-br)
	// - Princípio:
	// - O Assimp deve interpretar o formato de arquivo de entrada a partir do construtor da classe Model. Model é um container para estruturas de dados de vertices, indíces e texturas. Esses tipos de dados deverão ser alimentados a partir da biblioteca Assimp e então carregados para o tratamento por funções OpenGL adequadas.
	
	class Vertex{
	public:
		vec3<float> position;
		vec3<float> normal;
		vec2<float> texCoords;
	};

	class Texture{
	public:
		uint id;
		std::string type;
		aiString path;
	};

	class Mesh{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		std::vector<Texture> textures;
		Mesh(std::vector<Vertex> pVertices, std::vector<uint> pIndices, std::vector<Texture> pTextures);	

		void render(uint& program);
	private:
		uint VAO;
		uint VBO;
		uint IBO;
		void setup();	
	};

	class Model : public SRW{
	public:
		Model(std::string path, uint prog);
		void render();
	private:
		std::vector<Mesh> meshes;
		std::string directory;
		uint program;		
		void loadModel(std::string path);		
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);		
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	};
#endif