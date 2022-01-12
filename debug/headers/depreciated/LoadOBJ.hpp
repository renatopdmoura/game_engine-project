#ifndef LOADOBJ_H
#define LOADOBJ_H
#include "Structs.hpp"
#include "vec3.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

Light redTone("Red", vec3f(5.875f, -0.20f, 0.0f), vec3f(0.2f, 0.2f, 0.2f), vec3f(1.0f, 0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec3f(-0.2f, -10.0f, -0.3f), 1.0f, 0.09f, 0.032f);
Light greenTone("Green", vec3f(-5.875f, -0.20f, 0.0f), vec3f(0.2f, 0.2f, 0.2f), vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f);
Light cyanTone("Cyan", vec3f(0.0f, -0.20f, 5.875f), vec3f(0.2f, 0.2f, 0.2f), vec3f(0.0f, 1.0f, 1.0f), vec3f(0.0f, 1.0f, 1.0f), vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f);
Light whiteTone("White", vec3f(0.0f, 0.0, 1.0f), vec3f(0.2f, 0.2f, 0.2f), vec3f(1.0f, 1.0f, 1.0f), vec3f(1.0f, 1.0f, 1.0f), vec3f(-2.0f, -1.0f, 0.0f), 1.0f, 0.09f, 0.032f);
Light sun("Sun", vec3f(0.0f, 0.0f, 0.0f), vec3f(0.5f, 0.5f, 0.5f), vec3f(0.8f, 0.8f, 0.8f), vec3f(0.5f, 0.5f, 0.5f), vec3f(0.0f, -1.0f, -20.0f), 1.0f, 0.09f, 0.032f);

Material standart("Standart", vec3f(0.8f, 0.8f, 0.8f), vec3f(0.2f, 0.2f, 0.2f), vec3f(1.0f, 1.0f, 1.0f), 32.0f);
Material emerald("Emerald", vec3f(0.0215f, 0.1745f, 0.0215f), vec3f(0.07568f, 0.61424, 0.07568), vec3f(0.633f, 0.727811f, 0.633f), (0.6f * 128));
Material jade("Jad", vec3f(0.135f, 0.2225f, 0.1575f),vec3f(0.54f, 0.89f, 0.63f), vec3f(0.316228f, 0.316228f, 0.316228f), (0.1f * 128));
Material obsidian("Obsidian", vec3f(0.05375f, 0.05f, 0.06625f), vec3f(0.18275f, 0.17f, 0.22525f), vec3f(0.332741f, 0.328634f, 0.346435f), (0.3f * 128));
Material pearl("Pearl", vec3f(0.25f, 0.20725f, 0.20725f), vec3f(1.0f, 0.829f, 0.829f), vec3f(0.296648, 0.296648f, 0.296648f), (0.088 * 128));
Material ruby("Ruby", vec3f(0.1745f, 0.01175f, 0.01175f), vec3f(0.61424f, 0.04136f, 0.04136f), vec3f(0.727811f, 0.626959f, 0.626959f), (0.6f * 128));
Material turquoise("Turquoise", vec3f(0.1f, 0.18725f, 0.1745f), vec3f(0.396f, 0.74151f, 0.69102f), vec3f(0.297254f, 0.30829f, 0.306678f), (0.1f * 128));
Material brass("Brass", vec3f(0.329412f, 0.223529f, 0.027451f), vec3f(0.780392f, 0.568627f, 0.113725f), vec3f(0.992157f, 0.941176f, 0.807843f), (0.21794872 * 128));
Material bronze("Bronze", vec3f(0.2125f, 0.1275f, 0.054f), vec3f(0.714f, 0.4284f, 0.18144f), vec3f(0.393548f, 0.271906f, 0.166721f), (0.2f * 128));
Material chrome("Chrome", vec3f(0.25f, 0.25f, 0.25f), vec3f(0.4f, 0.4f, 0.4f), vec3f(0.774597f, 0.774597f, 0.774597f), (0.6 * 128));
Material copper("Copper", vec3f(0.19125f, 0.0735f, 0.0225f), vec3f(0.7038f, 0.27048f, 0.0828f), vec3f(0.256777f, 0.137622f, 0.086014f), (0.1f * 128));
Material gold("Gold", vec3f(0.24725f, 0.1995f, 0.0745f), vec3f(0.75164f, 0.60648f, 0.22648f), vec3f(0.628281f, 0.555802f, 0.366065f), (0.4f * 128));
Material silver("Silver", vec3f(0.19225f, 0.19225, 0.19225f), vec3f(0.507554f, 0.507554f, 0.50754f), vec3f(0.508273f, 0.508273f, 0.508273f), (0.4f * 128));
Material blackPlastic("BlackPlastic", vec3f(0.0f, 0.0f, 0.0f),	vec3f(0.01f, 0.01f, 0.01f), vec3f(0.50f, 0.50f, 0.50f), (0.25f * 128));
Material cyanPlastic("CyanPlastic", vec3f(0.0f, 0.1f, 0.06f), vec3f(0.0f, 0.50980392f, 0.50980392f), vec3f(0.50196078f, 0.50196078f, 0.50196078f), (0.25f * 128));
Material greenPlastic("GreenPlastic", vec3f(0.0f, 0.0f, 0.0f),	vec3f(0.1f, 0.35f, 0.1f), vec3f(0.45f, 0.55f, 0.45f), (0.25f * 128));
Material redPlastic("RedPlastic", vec3f(0.0f, 0.0f, 0.0f), vec3f(0.5f, 0.0f, 0.0f), vec3f(0.7f, 0.6f, 0.6f), (0.25 * 128));
Material whitePlastic("WhitePlastic", vec3f(0.0f, 0.0f, 0.0f),	vec3f(0.55f, 0.55f, 0.55f), vec3f(0.70f, 0.70f, 0.70f), (0.25f * 128));
Material yellowPlastic("YellowPlastic", vec3f(0.0f, 0.0f, 0.0f), vec3f(0.5f, 0.5f, 0.0f), vec3f(0.60f, 0.60f, 0.50f), (0.25f * 128));
Material blackRubber("BlackRubber", vec3f(0.02f, 0.02f, 0.02f), vec3f(0.01f, 0.01f, 0.01f), vec3f(0.4f,	0.4f, 0.4f), (0.078125f * 128));
Material cyanRubber("CyanRubber", vec3f(0.0f, 0.05f, 0.05f), vec3f(0.4f, 0.5f, 0.5f), vec3f(0.04f, 0.7f, 0.7f), (0.078125f * 128));
Material greenRubber("GreenRubber", vec3f(0.0f, 0.05f, 0.0f), vec3f(0.4f, 0.5f, 0.4f), vec3f(0.04f, 0.7f, 0.04f), (0.078125f * 128));
Material redRubber("RedRubber", vec3f(0.05f, 0.0f, 0.0f), vec3f(0.5f, 0.4f, 0.4f), vec3f(0.7f, 0.04f, 0.04f), (0.078125 * 128));
Material whiteRubber("WhiteRubber", vec3f(0.05f, 0.05f, 0.05f), vec3f(0.5f,	0.5f, 0.5f), vec3f(0.7f, 0.7f, 0.7f), (0.078125 * 128));
Material yellowRubber("YellowRubber", vec3f(0.05f, 0.05f, 0.0f), vec3f(0.5f, 0.5f, 0.4f), vec3f(0.7f, 0.7f,	0.04f), (0.078125f * 128));

std::vector<Material> presetsMaterial;
void setArrayMaterial(){
	presetsMaterial.push_back(standart);
	presetsMaterial.push_back(emerald);
	presetsMaterial.push_back(jade);
	presetsMaterial.push_back(obsidian);
	presetsMaterial.push_back(pearl);
	presetsMaterial.push_back(ruby);
	presetsMaterial.push_back(turquoise);
	presetsMaterial.push_back(brass);
	presetsMaterial.push_back(bronze);
	presetsMaterial.push_back(chrome);
	presetsMaterial.push_back(copper);
	presetsMaterial.push_back(gold);
	presetsMaterial.push_back(silver);
	presetsMaterial.push_back(blackPlastic);
	presetsMaterial.push_back(cyanPlastic);
	presetsMaterial.push_back(greenPlastic);
	presetsMaterial.push_back(redPlastic);
	presetsMaterial.push_back(whitePlastic);
	presetsMaterial.push_back(yellowPlastic);
	presetsMaterial.push_back(blackRubber);
	presetsMaterial.push_back(cyanRubber);
	presetsMaterial.push_back(greenRubber);
	presetsMaterial.push_back(redRubber);
	presetsMaterial.push_back(whiteRubber);
	presetsMaterial.push_back(yellowRubber);
}
Material* searchMaterialByName(std::string keyWord){
	unsigned int i = 0;
	while(i < presetsMaterial.size()){
		if(keyWord == presetsMaterial[i].getName()){
			return &presetsMaterial[i];
		}
		++i;
	}
	std::cout << "Material <" << keyWord << "> no found! Returned default." << std::endl;
	return &presetsMaterial[0];
}

//Usadas pela função parserOBJ na classe OBJ
struct Vertex{
	vec3f position;
	vec2f texcoord;
	vec3f normal;
};

void search(std::vector<float>* container, std::ifstream* file, char code){
	std::string strData;
	bool readLine = true;
	while(readLine){
		file->get(code);
		while(code != ' '){
			strData += code;
			file->get(code);
			readLine = code != '\n'? true: false;
			if(!readLine) break;
		}
		if(!strData.empty()){
			container->push_back(std::stof(strData));
			strData.clear();
		}
	}	
}

struct OBJATTRIB{
	GLuint 		diffuse;
	GLuint 		specular;
	float 		shininess;
	mat4<float> offset;
};

class OBJ{
public:
	OBJ(const char* skybox){
		initialize();		
		if(skybox == "CREATE_SKYBOX"){
			if(parserOBJ("models/cube.obj")){
				vertexBytes = sizeof(GLfloat) * vertexStream.size();
				indexBytes  = sizeof(GLuint)  * indexStream.size();
				currentShader.create("../shaders/vs_cubemap.glsl.c", "../shaders/fs_cubemap.glsl.c");
				generate();
			}
		}
	}
	OBJ(const char* pathModel, const char* vs_path, const char* fs_path, GLuint* parm = NULL){
		initialize();		
		if(parserOBJ(pathModel)){
			vertexBytes = sizeof(GLfloat) * vertexStream.size();
			indexBytes  = sizeof(GLuint)  * indexStream.size();
			if(parm != NULL){
				currentShader.program = *parm;
				currentShader.fsVersion(fs_path);
			}
			else{
				currentShader.create(vs_path, fs_path);
			}
			generate();
		}
	}
	bool initialize(){
		mat4<float> identity;
		model       = &identity;
		view        = &identity;
		cameraSpace = &identity;
	}
	bool parserOBJ(const char* path){
		std::ifstream file(path, std::ios_base::in);
		if(!file.is_open()){
			std::cout << "Failed to open file " << path << std::endl;
			return false;
		}
		else{
			std::string strData;
			char code;
			bool readLine = false;
			bool indexv   = false;
			bool indexn   = false;
			bool indext   = false;
			unsigned int slash  = 0;
			std::vector<Vertex> vertexArray;
			std::vector<float> position;
			std::vector<float> color;
			std::vector<float> normal;
			std::vector<float> texture;
			std::vector<int> indexVertexStream;
			std::vector<int> indexNormalStream;
			std::vector<int> indexTextureStream;
			std::vector<vec3f> stream;
			std::vector<vec3f> pos;
			std::vector<vec3f> norm;
			std::vector<vec3f> tex;
			while(file.get(code)){
				switch(code){
					case 'o':
						file.get(code);
						if(code == ' '){
							while(code != '\n'){
								file.get(code);
								name += code;
							}
							name.pop_back();
						}
					break;
					case 'v':
						file.get(code);
							if(code == ' '){
								search(&position, &file, code);
							}
							else if(code == 'n'){
								search(&normal, &file, code);
							}
							else if(code == 't'){
								search(&texture, &file, code);
							}
					break;
					case 'f':
						file.get(code);
						if(code == ' '){
							readLine = true;
							while(readLine){
								if(code == ' '){
									slash = 0;
									indexv = true;
									file.get(code);
								}
								if(code == '/'){
									slash += 1;
									if(slash == 1) indext = true;
									if(slash == 2) indexn = true;
									file.get(code);
								}
								while(code != ' ' && code != '/'){
									strData += code;
									file.get(code);
									readLine = code != '\n'? true: false;
									if(!readLine) break;
								}
								if(!strData.empty()){
									if(indexv){
										indexv = false;
										indexVertexStream.push_back(std::stoi(strData) - 1);
										strData.clear();
									}
									if(indexn){
										indexn = false;
										indexNormalStream.push_back(std::stoi(strData) - 1);
										strData.clear();
									}
									if(indext){
										indext = false;
										indexTextureStream.push_back(std::stoi(strData) - 1);
										strData.clear();
									}
								}
							}
						}
					break;
				}
			}
			unsigned int i = 0;
			unsigned int j = 0;
			unsigned int k = 0;
			while(i < position.size()){
				vec3f temp;
				temp.x = position[i];
				temp.y = position[i + 1];
				temp.z = position[i + 2];
				pos.push_back(temp);
				i += 3;
			}
			while(j < normal.size()){
				vec3f temp;
				temp.x = normal[j];
				temp.y = normal[j + 1];
				temp.z = normal[j + 2];
				norm.push_back(temp);
				j += 3;
			}
			while(k < texture.size()){
				vec3f temp;
				temp.x = texture[k];
				temp.y = texture[k + 1];
				tex.push_back(temp);
				k += 2;
			}
			unsigned int tot = indexVertexStream.size() + indexNormalStream.size() + indexTextureStream.size();
			unsigned int index = 0;
			unsigned int iv = 0;
			unsigned int in = 0;
			unsigned int it = 0;
			while(index < tot){
				for(unsigned i = 0; i < 3; ++i){
					if(iv < indexVertexStream.size()){
						stream.push_back(pos[indexVertexStream[iv]]);
						iv++;
						index++;
					}
				}
				for(unsigned i = 0; i < 3; ++i){
					if(in < indexNormalStream.size()){
						stream.push_back(norm[indexNormalStream[in]]);
						in++;
						index++;
					}
				}
				for(unsigned i = 0; i < 3; ++i){
					if(it < indexTextureStream.size()){
						stream.push_back(tex[indexTextureStream[it]]);
						it++;
						index++;
					}
				}
			}
			index = 0;
			unsigned int cv = 0;
			unsigned int cn = 3;
			unsigned int ct = 6;
			unsigned int count = 0;
			while(count < stream.size()){
					Vertex temp;
					temp.position = stream[cv];
					cv++;
					count++;
					temp.normal = stream[cn];
					cn++;
					count++;
					temp.texcoord.x = stream[ct].x;
					temp.texcoord.y = stream[ct].y;
					ct++;
					count++;
					vertexArray.push_back(temp);			
					index++;
					if(index == 3){
						cv += 6;
						cn += 6;
						ct += 6;
						index = 0;
					}
			}
			for(unsigned i = 0; i < vertexArray.size(); ++i){
				vertexStream.push_back(vertexArray[i].position.x);
				vertexStream.push_back(vertexArray[i].position.y);
				vertexStream.push_back(vertexArray[i].position.z);
				vertexStream.push_back(vertexArray[i].normal.x);
				vertexStream.push_back(vertexArray[i].normal.y);
				vertexStream.push_back(vertexArray[i].normal.z);
				vertexStream.push_back(vertexArray[i].texcoord.x);
				vertexStream.push_back(vertexArray[i].texcoord.y);
			}
			if(false){
				if(true){
					int linebreak = 0;
					for(unsigned i = 0; i < vertexStream.size(); ++i){
						linebreak++;
						std::cout << std::setw(5) << vertexStream[i] << " ";
						if(linebreak == 8){
							std::cout << std::endl;
							linebreak = 0;
						}
					}
					std::cout << std::endl;
				}
				size_t bytes = vertexStream.size() * sizeof(float);
				std::cout << name << " | vertex stream bytes: " << bytes / (1024.0f * 1024.0f) << "MB from " << vertexStream.size() << " elements." << std::endl;
			}
			for(unsigned i = 0; i < vertexStream.size() / 3; ++i){
				indexStream.push_back(i);
			}
		}
		return true;
	}
	//need position and materials
	void instantiate(unsigned int instance_count, mat4<float> offsets[]){
		// glDeleteProgram(currentShader.program);//Zera parm
		instanceCount  = instance_count;
		
		std::ofstream write_vs_shader("../shaders/vs_instance.glsl.c", std::ios_base::trunc | std::ios_base::binary);
		std::string instaceLine("	mat4 offset[" + std::to_string(instance_count) +"];\n");
		std::vector<std::string> data = { 	"#version 460\n",
										  	"layout(location = 0) in vec4 position;\n", 
										  	"layout(location = 1) in vec4 normal;\n",
										  	"layout(location = 2) in vec2 uvcoord;\n",
										  	"\n",
										  	"out struct VS_OUT{\n",
										  	"	vec2 uvcoord;\n",
										  	"	vec3 normal;\n",
										  	"	vec3 fragpos;\n",
										  	"	mat4 cameraSpace;\n",
										  	"	mat4 model;\n",
										  	"}vs_out;\n",
										  	"\n",
										  	"layout(binding = 0) uniform objAttrib{\n",
										  		instaceLine,
										  	"}offset_array;\n",
										  	"\n",
										  	"uniform mat4 model;\n",
										    "uniform mat4 viewMatrix;\n",
											"uniform mat4 cameraSpace;\n",
										  	"\n",
										    "void main(){\n",
										    "	mat4 transform     = offset_array.offset[gl_InstanceID] * model;\n",
										 	"	gl_Position        = viewMatrix * transform * position;\n",
											"	vs_out.normal      = normalize(vec3(transpose(inverse(cameraSpace * transform)) * normal));\n",
											"	vs_out.fragpos     = vec3((cameraSpace * transform) * position);\n",
											"	vs_out.uvcoord     = uvcoord;\n",
											"	vs_out.cameraSpace = cameraSpace;\n",
											"	vs_out.model       = transform;\n"
											"}"
										};
		for(unsigned i = 0; i < data.size(); ++i)
			write_vs_shader << data[i].c_str();	
		write_vs_shader.close();		

		if(currentShader.fragmentShaderVersion == "OBJ_USE_TEXTURE" ){
			currentShader.create("../shaders/vs_instance.glsl.c", "../shaders/fs_texture.glsl.c");
		}
		else if(currentShader.fragmentShaderVersion == "OBJ_USE_MATERIAL"){
			currentShader.create("../shaders/vs_instance.glsl.c", "../shaders/fs_material.glsl.c");
		}
		
		GLuint blockIndex = glGetUniformBlockIndex(currentShader.program, "objAttrib");
		if(blockIndex == GL_INVALID_INDEX){
			std::cout << "Error: Uniform block name is not have a valid index!" << std::endl;
		}
		else{
			GLint blockSize = -1;
			glGetActiveUniformBlockiv(currentShader.program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			if(blockSize == -1){
				std::cout << "Error: An error occurred getting the minimum size for the buffer!" << std::endl;
			}
			else{
				// std::cout << "Minimum size buffer object: " << blockSize << " bytes." << std::endl;
				GLubyte* blockBuffer = new GLubyte[blockSize];
				GLuint countMembers = 1;
				const GLchar* names[] = {"objAttrib.offset"};
				GLuint indices[countMembers];
				glGetUniformIndices(currentShader.program, countMembers, names, indices);
				bool success = true;
				for(unsigned index = 0; index < countMembers; ++index){
					switch(indices[index]){
						case GL_INVALID_INDEX:
							std::cout << names[index] << " is not a program variable shader or is not active!" << std::endl;
							success = false;
						break;
					}
				}
				if(success){
					GLint offset[countMembers];
					glGetActiveUniformsiv(currentShader.program, countMembers, indices, GL_UNIFORM_OFFSET, offset);
					//Data to fill buffer
					for(unsigned i = 0; i < instanceCount; ++i){
						std::memcpy(blockBuffer + (sizeof(offsets[i]) * i), offsets[i].matrix, 16 * sizeof(float));
					}
											
					//Create UBO
					glGenBuffers(1, &ubo);
					glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
					glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
					//Map buffer
					if(false){
						std::cout << "Buffer content:" << std::endl;
						int count = 0;
						int brk = 0;
						unsigned int array_size = (blockSize * sizeof(float)) / 16;
						float* ptr = new float[array_size];
						ptr = reinterpret_cast<float*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY));
						if(ptr != NULL){
							for(unsigned i = 0; i < array_size; i++){
								std::cout << ptr[i] << " ";
								if(count == 3){
									count = 0;
									std::cout << std::endl;
								}
								else{
									count++;
								}
								if(brk == 15){
									brk = 0;
									std::cout << std::endl;
								}
								else
									brk++;
							}
						}
						glUnmapBuffer(GL_UNIFORM_BUFFER);
					}
				}
			}
		}
	}
	void generate(){
		glGenBuffers(1, &vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vertexBytes, reinterpret_cast<void*>(vertexStream.data()), GL_STREAM_DRAW);
		glGenBuffers(1, &indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, reinterpret_cast<void*>(indexStream.data()), GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
		
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		currentShader.setVertexAttribPointer();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBindVertexArray(0);

		vertexStream.clear();
		indexStream.clear();
	}
	void renderMatrices(GLint program){
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, reinterpret_cast<float*>(model->matrix));
		glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_FALSE, reinterpret_cast<float*>(view->matrix));
		glUniformMatrix4fv(glGetUniformLocation(program, "cameraSpace"), 1, GL_FALSE, reinterpret_cast<float*>(cameraSpace->matrix));
	}
	void render(){
		glUseProgram(currentShader.program);
			glBindVertexArray(vertexArrayObject);
			currentShader.initUniforms();
			renderMatrices(currentShader.program);
			glDrawElements(GL_TRIANGLES, indexBytes / sizeof(GLuint), GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		glUseProgram(0);
	}
	void render(unsigned int instance_count){
		glUseProgram(currentShader.program);
		glBindVertexArray(vertexArrayObject);
		currentShader.initUniforms();
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		renderMatrices(currentShader.program);
		glDrawElementsInstanced(GL_TRIANGLES, indexBytes / sizeof(GLuint), GL_UNSIGNED_INT, NULL, instanceCount);
		glUseProgram(0);
	}
	void setModelMatrix(mat4<float>* matrix){
		this->model = matrix;
	}
	void setViewMatrix(mat4<float>* matrix){
		this->view = matrix;
	}
	void setCameraSpace(mat4<float>* matrix){
		this->cameraSpace = matrix;
	}
	void bindScene(Scene *parm){
		this->currentShader.scene = parm;
	}
	Shader& shader(){
		return currentShader;
	}
	void free(){
		currentShader.free();
		name.clear();
		vertexBytes   = 0;
		indexBytes    = 0;
		instanceCount = 0;
		model         = NULL;
		view          = NULL;
		cameraSpace   = NULL;
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(1, &vertexBufferObject);
		glDeleteBuffers(1, &indexBufferObject);
		glDeleteBuffers(1, &ubo);
	}
private:
	//basic resources
	Shader currentShader;
	std::string name;
	std::vector<GLfloat> vertexStream;
	std::vector<GLuint> indexStream;
	size_t vertexBytes;
	size_t indexBytes;
	GLuint vertexArrayObject;
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
	mat4<float>* model;
	mat4<float>* view;
	mat4<float>* cameraSpace;
	//instance resource
	GLuint ubo;
	unsigned int instanceCount;
};

/*
Essa é uma função experimental, no atual ponto de desenvolvimento
sua eficiência é relativa. O desempenho resulta em bytes
significativos de mémoria se comparado a forma de desenho matricial e para objetos mais complexos - mais poligonos -
a pesquisa linear atrasa a execução do programa devido a necessidade de percorrer os elementos do buffer um a um.
*/
GLuint* parserOBJ(const char* path){
	bool success = true;
	//Carrega as informações do objeto
	std::ifstream file(path, std::ios_base::binary);
	std::vector<vec3f> positionBuffer;
	std::vector<unsigned int> indexBuffer;
	if(file.is_open()){
		std::filebuf* out_buffer = file.rdbuf();
		char code = '\0'; 
		bool VERTEX = false;
		bool FACES = false;
		while(code != EOF){
			code = out_buffer->sbumpc();
			if(code == 'v' && out_buffer->sbumpc() == ' '){
				VERTEX = true;
				std::string data;
				int count = 0;
				vec3f coordinate;
				while(VERTEX){
					code = out_buffer->sbumpc();
					while(code != ' ' && code != '\n'){
						data += code;
						code = out_buffer->sbumpc();
						VERTEX = code != '\n'? true: false;
					}
					switch(count){
						case 0:
							coordinate.x = std::stof(data);
							count++;
						break;
						case 1:
							coordinate.y = std::stof(data);
							count++;
						break;
						case 2:
							coordinate.z = std::stof(data);
							count++;
						break;
					}
					if(count == 3) positionBuffer.push_back(coordinate);
					data.clear();
				}
			}
			if(code == 'f' && out_buffer->sbumpc() == ' '){
				FACES = true;
				std::string data;
				while(FACES){
					code = out_buffer->sbumpc();
					while(code != ' ' && code != '\n'){
						data += code;
						code = out_buffer->sbumpc();
						FACES = code != '\n'? true: false;
					}
					indexBuffer.push_back(std::stoi(data) - 1);
					data.clear();
				}
			}
		}	
	file.close();
	}
	else
		success = false;
	// Calcula as normais e corrige o buffer
	if(success){
		std::vector<vec3f> vertexStream;
		std::vector<vec3f> normalStream;
		std::vector<GLuint> indexStream;

		for(unsigned int i = 0; i < indexBuffer.size(); i += 3){
			vec3f v[3];
			v[0] = positionBuffer[indexBuffer[i + 0]];
			v[1] = positionBuffer[indexBuffer[i + 1]];
			v[2] = positionBuffer[indexBuffer[i + 2]];
			vec3f result = normalize(cross(v[1] - v[0], v[2] - v[0]));

			if(vertexStream.size() == 0){
			 	indexStream.push_back(vertexStream.size());
			 	vertexStream.push_back(v[0]);
			 	normalStream.push_back(result);

			 	indexStream.push_back(vertexStream.size());
			 	vertexStream.push_back(v[1]);
			 	normalStream.push_back(result);
			 	
			 	indexStream.push_back(vertexStream.size());
			 	vertexStream.push_back(v[2]);
			 	normalStream.push_back(result);
			}
			else{
				bool EQUAL = false;
				for(unsigned int j = 0; j < 3; ++j){
					//break while se: falso se se houver vértice igual ou se chegar ao final do vetor vs
					//break while se: false se houver normal igual ou se chegar ao final do vetor ns
					unsigned int n = 0;
					while(n < vertexStream.size()){
						if(notequalf(v[j], vertexStream[n]) || notequalf(result, normalStream[n])){
							EQUAL = false;
							n++;
						}
						else{
							indexStream.push_back(n);
							EQUAL = true;
							break;
						}
					}
					if(!EQUAL){
						indexStream.push_back(vertexStream.size());
						vertexStream.push_back(v[j]);
						normalStream.push_back(result);
					}
				}
			}
		}
		for(unsigned int i = 0; i < vertexStream.size(); ++i){			
			vertexStream[i].show((std::to_string(i) + std::string(" vertexStream")).c_str());
		}
		for(unsigned int i = 0; i < normalStream.size(); ++i){			
			normalStream[i].show((std::to_string(i) + std::string(" normalStream")).c_str());
		}
		std::cout << indexStream.size() << std::endl;
		for(unsigned int i = 0; i < indexStream.size(); ++i){			
			std::cout << indexStream[i] << " ";
		}
		std::vector<float> buffer;
		for(unsigned int i = 0; i < vertexStream.size(); ++i){
			buffer.push_back(vertexStream[i].x);
			buffer.push_back(vertexStream[i].y);
			buffer.push_back(vertexStream[i].z);
			buffer.push_back(normalStream[i].x);
			buffer.push_back(normalStream[i].y);
			buffer.push_back(normalStream[i].z);
		}
		std::cout << "meabytes:" << (sizeof(GLfloat) * buffer.size()) / (1024.0f * 1024.0f) << std::endl;
		system("pause");
		system("cls");
		// Configura os parâmetros para renderização
		GLuint* GL_OBJ = new GLuint[4];
		glGenVertexArrays(1, &GL_OBJ[0]);
		glBindVertexArray(GL_OBJ[0]);
		glGenBuffers(1, &GL_OBJ[1]);
		glBindBuffer(GL_ARRAY_BUFFER, GL_OBJ[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * buffer.size(), reinterpret_cast<void*>(buffer.data()), GL_STATIC_DRAW);
		glGenBuffers(1, &GL_OBJ[2]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_OBJ[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexStream.size(), &(indexStream[0]), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, reinterpret_cast<void*>(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, reinterpret_cast<void*>(3 * sizeof(float)));
		glBindVertexArray(0);
		GL_OBJ[4] = indexStream.size();
		return &GL_OBJ[0];
	}
	return NULL;
}

#endif