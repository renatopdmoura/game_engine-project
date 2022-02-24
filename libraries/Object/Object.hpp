#ifndef OBJECT_H
#define OBJECT_H

#include "../Common/Common.hpp"
#include "../Math/Math.hpp"
#include "../SRW/SRW.hpp"
#include "../Camera/Camera.hpp"
#include "../Text/Text.hpp"

// - Standart header files
#include <glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// - Assimp header files
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

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
			static void select(Camera& camera, SDL_Event* event, std::vector<Text>& objList);
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
		void render(uint instances);

		// - Setters
		void setName(std::string pname);

		// - Getters		
		uint getVAO() const;
		uint getVBO() const;
		uint getUBO() const;
		uint getShaderType() const;
		std::string getName() const;
		std::vector<float> getVertexBufferArray();
		mat4<float>& getModel();
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
		uint VAO;
		uint VBO;
		uint UBO;
		uint shaderType;
		uint instanceMethod;
		std::string name;
		std::vector<float> vertex;
		mat4<float> model;
		std::vector<mat4<float>>* arrayModel;

		// - Posições relativas(objeto e observador) anteriores a uma detecção de colisão
		vec3<float> cameraBeforeCollision;
		mat4<float> modelBeforeCollision;
		
		// - Usar uma pilha de matrizes requer poder determinar a ordem de operações de escala, rotação e translação
		std::vector<mat4<float>> transforms;
	};


	// - New way to load 3d models using Assimp

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
	};

	class Mesh{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		std::vector<Texture> textures;
		Mesh(std::vector<Vertex> pVertices, std::vector<uint> pIndices, std::vector<Texture> pTextures){
			vertices = pVertices;
			indices  = pIndices;
			textures = pTextures;
		}

		void render(){
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	private:
		uint VAO;
		uint VBO;
		uint IBO;

		void setup(){
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0); // - positions
			glEnableVertexAttribArray(1); // - normals
			glEnableVertexAttribArray(2); // - texxture coordinates
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(vec3<float>)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(vec3<float>) * 2));
			glBindVertexArray(0);
		}
	};

	class Model{
	public:
		Model(std::string path){
			loadModel(path);
		}
		void render(){
			for(std::vector<Mesh>::iterator it = meshes.begin(); it != meshes.end(); ++it){
				(*it).render();
			}
		}
	private:
		std::vector<Mesh> meshes;
		std::string directory;
		
		void loadModel(std::string path){
			Assimp::Importer import;
			const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
			if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
				std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			}
			processNode(scene->mRootNode, scene);
		}
		
		void processNode(aiNode *node, const aiScene *scene){
			for(uint i = 0; i < node->mNumMeshes; ++i){
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			for(uint i = 0; i < node->mNumChildren; ++i){
				processNode(node->mChildren[i], scene);
			}
		}

		Mesh processMesh(aiMesh *mesh, const aiScene *scene){
			std::vector<Vertex> vertices;
			std::vector<uint> indices;
			std::vector<Texture> textures;
			for(uint i = 0; i < mesh->mNumVertices; ++i){
				Vertex vertex;
				vec3<float> vector;
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.position = vector;
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
				if(mesh->mTextureCoords[0]){
					vec2<float> vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.texCoords = vec;
				}
				else
					vertex.texCoords = vec2<float>(0.0f, 0.0f);
			}
			for(uint i = 0; i < mesh->mNumFaces; ++i){
				aiFace face = mesh->mFaces[i];
				for(uint j = 0; j < face.mNumIndices; ++j)
					indices.push_back(face.mIndices[j]);
			}
			return Mesh(vertices, indices, textures);

		}
		
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	};

#endif