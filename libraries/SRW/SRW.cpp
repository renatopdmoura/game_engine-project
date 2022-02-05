#include "SRW.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../libraries/stbi/stb_image.h"

#if RENDER_DEBUG_MODE
	uint SRW::debugPrograms[] = {0, 0};
#endif

uint SRW::programs[] = {0, 0, 0, 0, 0, 0};
std::vector<uint> SRW::gUBO(3);

//Generate a program variable that can shared between objects
uint SRW::genProgramShader(std::string vs_path, std::string fs_path){
	uint prog = 0;
	if(createProgram(prog)){
		bool vs_state = createShader(prog, GL_VERTEX_SHADER, readShaderSource(vs_path.c_str()));
		bool fs_state = createShader(prog, GL_FRAGMENT_SHADER, readShaderSource(fs_path.c_str()));
		try{
			if(!vs_state || !fs_state) throw 0;
			else if(!linkProgram(prog))	throw 1;
		}
		catch(const int exception){
			switch(exception){
				case 0: std::cerr << "Exception: Failed to create shader!\n"; break; 
				case 1: std::cerr << "Exception: Failed to link to program!"; break;
			}
			exit(1);
		}
	}
	return prog;
}

uint SRW::genProgramShader(std::string vs_path, std::string gs_path, std::string fs_path){
	uint prog = 0;
	if(createProgram(prog)){
		bool vs_state = createShader(prog, GL_VERTEX_SHADER, readShaderSource(vs_path.c_str()));
		bool gs_state = createShader(prog, GL_GEOMETRY_SHADER, readShaderSource(gs_path.c_str()));
		bool fs_state = createShader(prog, GL_FRAGMENT_SHADER, readShaderSource(fs_path.c_str()));
		try{
			if(!vs_state || !gs_state || !fs_state) throw 0;
			else if(!linkProgram(prog))	throw 1;
		}
		catch(const int exception){
			switch(exception){
				case 0: std::cerr << "Exception: Failed to create shader!\n"; break; 
				case 1: std::cerr << "Exception: Failed to link to program!"; break;
			}
			exit(1);
		}
	}
	return prog;
}

void SRW::genProgramShaders(){
	SRW::programs[UNIFORM_COLOR]   = SRW::genProgramShader("../shaders/object/vs.glsl", "../shaders/object/fs_uniform_color.glsl");
	SRW::programs[TEXTURIZED]      = SRW::genProgramShader("../shaders/object/vs.glsl", "../shaders/object/fs_textured.glsl");
	SRW::programs[TEXT_PROGRAM]    = SRW::genProgramShader("../shaders/new/vs.glsl", "../shaders/new/fs.glsl");
	SRW::programs[TERRAIN_PROGRAM] = SRW::genProgramShader("../shaders/terrain/vs.glsl", "../shaders/terrain/fs.glsl");
	SRW::programs[GUI_PROGRAM]     = SRW::genProgramShader("../shaders/ui/vs_gui.glsl", "../shaders/ui/fs_gui.glsl");
	SRW::programs[GUI_PANEL_PROG]  = SRW::genProgramShader("../shaders/ui/vs_gui.glsl", "../shaders/ui/fs_panel.glsl");
	#if RENDER_DEBUG_MODE
		debugPrograms[0] = SRW::genProgramShader("../shaders/debug/vs_debug.glsl", "../shaders/debug/fs_debug_depth.glsl");
		debugPrograms[1] = SRW::genProgramShader("../shaders/debug/vs_debug_normal.glsl", "../shaders/debug/gs_debug_normal.glsl", "../shaders/debug/fs_debug_normal.glsl");
	#endif
}

void SRW::genShader(std::string vs_path, std::string fs_path){
	if(createProgram(program)){
		bool vs_state = createShader(program, GL_VERTEX_SHADER, readShaderSource(vs_path.c_str()));
		bool fs_state = createShader(program, GL_FRAGMENT_SHADER, readShaderSource(fs_path.c_str()));
		try{
			if(!vs_state || !fs_state) throw 0;
			else if(!linkProgram(program)) throw 1;
		}
		catch(const int exception){
			switch(exception){
				case 0: std::cerr << "Exception: Failed to create shader!\n"; break; 
				case 1: std::cerr << "Exception: Failed to link to program!"; break;
			}
			exit(1);
		}
	}
}

void SRW::genGeometryBuffer(uint& gBuffer, uint& gDepth, uint& gPosition, uint& gNormal, uint& gAlbedo, uint width, uint height){
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	uint attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2}; 
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);	

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "Warning: the framebuffer is incomplete!" << std::endl;
		switch(framebufferStatus){
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
			break;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SRW::sharedUniforms(mat4<float>& viewProj, vec3<float>& cameraPos, bool update){
	if(update){
		uint prog = SRW::programs[TEXTURIZED];
		uint blockIndex = glGetUniformBlockIndex(prog, "Var");
		if(blockIndex == GL_INVALID_INDEX)
			std::cerr << "ERROR::Uniformblock <Var> is not have a valid index!" << std::endl;
		else{
			int blockSize = -1;
			glGetActiveUniformBlockiv(prog, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			if(blockSize == -1)
				std::cerr << "ERROR::An error occurred getting the minimum size for the buffer!" << std::endl;
			else{
				unsigned char* buffer = new unsigned char[blockSize];
				const char* member[]  = {"viewProj", "cameraPos"};
				uint count = 2;
				uint indices[count];
				glGetUniformIndices(prog, count, member, indices);
				for(uint i = 0; i < count; ++i){
					if(indices[i] == GL_INVALID_INDEX){
						std::cerr << member[i] << " is not a program variable or is not active!" << std::endl;
						exit(1);
					}
				}
				std::memcpy(buffer, &(viewProj)[0][0], sizeof(mat4<float>));
				std::memcpy(buffer + sizeof(mat4<float>), &(cameraPos), sizeof(vec4<float>));
				SRW::gUBO.push_back(0);
				glGenBuffers(1, &SRW::gUBO.back());
				glBindBufferBase(GL_UNIFORM_BUFFER, 3, SRW::gUBO.back());
				glBufferData(GL_UNIFORM_BUFFER, blockSize, buffer, GL_DYNAMIC_DRAW);
				if(false){
					float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY));
					if(ptr != NULL){
						vec4<float>(ptr[0], ptr[1], ptr[2], ptr[3]).show("viewProj");
						vec4<float>(ptr[4], ptr[5], ptr[6], ptr[7]).show("viewProj");
						vec4<float>(ptr[8], ptr[9], ptr[10], ptr[11]).show("viewProj");
						vec4<float>(ptr[12], ptr[13], ptr[14], ptr[15]).show("viewProj");
						vec4<float>(ptr[16], ptr[17], ptr[18], ptr[19]).show("cameraPos");

					}
					delete[] buffer;//?????
				}
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			}
		}
	}
	else{
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, SRW::gUBO.back());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4<float>) + sizeof(vec4<float>), &(viewProj)[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4<float>), sizeof(vec4<float>), &(cameraPos));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

void SRW::sharedUniforms(Sun* sun, std::vector<PointLight>* pointLightArray, std::vector<SpotLight>* spotLightArray){
	// - Para cada shader que usa o bloco uniforme, abri-lôs para determinar a quantidade de luzes lidas pelas funções
	// de iluminação
	std::vector<std::string> paths = {"../shaders/object/fs_textured.glsl", "../shaders/object/fs_uniform_color.glsl"};
	for(uint i = 0; i < paths.size(); ++i){
		std::fstream file(paths[i].c_str(), std::ios_base::in);
		if(!file.is_open()){
			std::cerr << "Unable to open file" << paths[i] << std::endl;
		}
		else{
			std::vector<std::string> strData;
			std::string str;
			while(std::getline(file, str)){
				if(!str.compare("// - index size point lights")){
					if(pointLightArray != NULL){
						strData.push_back(str);
						std::getline(file, str);
						str = "const int COUNT_POINT_LIGHT = " + std::to_string(pointLightArray->size()) + ";";
					}
					else{
						strData.push_back(str);
						std::getline(file, str);
						str = "const int COUNT_POINT_LIGHT = 0;";
					}
					if(spotLightArray != NULL){
						strData.push_back(str);
						std::getline(file, str);
						str = "const int COUNT_SPOT_LIGHT = " + std::to_string(spotLightArray->size()) + ";";
					}
					else{
						strData.push_back(str);
						std::getline(file, str);
						str = "const int COUNT_SPOT_LIGHT = 0;";
					}
				}
				if(pointLightArray != NULL){
					if(!str.compare("layout(binding = 0, shared) uniform PointLight{")){
						strData.push_back(str);
						std::getline(file, str);
						std::getline(file, str);	
						str = "\tvec4 position[" + std::to_string(pointLightArray->size()) + "];\n\tvec4 color[" + std::to_string(pointLightArray->size()) + "];";
					}
				}
				if(spotLightArray != NULL){
					if(!str.compare("layout(binding = 1, shared) uniform SpotLight{")){
						strData.push_back(str);
						std::getline(file, str);
						std::getline(file, str);
						std::getline(file, str);
						str = "\tvec4 direction[" + std::to_string(spotLightArray->size()) + "];\n\tvec4 position[" + std::to_string(spotLightArray->size()) + "];\n\tvec4 color[" + std::to_string(spotLightArray->size()) + "];";
					}
				}
				strData.push_back(str);
			}
			file.close();
			file.open(paths[i].c_str(), std::ios_base::out | std::ios_base::trunc);
			for(uint i = 0; i < strData.size(); ++i){
			 file << strData[i] << std::endl;
			}
			file.close();
		}
	}
	SRW::genProgramShaders();
	//- Obtém o bloco uniforme de um programa para determinar os deslocamentos dos dados dentro do bloco uniforme
	//- Note que qualquer programa que contenha o bloco uniforme esperado pode ser tomado por base para as operações
	//- Por questões de projeto, o ponto de ligação 0 corresponde ao armazenamento de luzes
	uint prog = SRW::programs[TEXTURIZED];
	std::vector<std::string> uniformBlockName  = {"PointLight", "SpotLight", "Sun"};
	const char* pointLightMemberName[] = {"PointLight.position", "PointLight.color"};
	const char* spotLightMemberName[]  = {"SpotLight.direction", "SpotLight.position", "SpotLight.color"};
	const char* sunLightMemberName[]   = {"Sun.direction", "Sun.color"};
	std::vector<uint> uniformBlockCountMembers = {2, 3, 2};
	std::vector<uint> uniformBlockIndex(3);
	std::vector<uint> uniformBlockMemberIndex;
	std::vector<int> uniformBlockSize(uniformBlockName.size());
	
	for(uint j = 0; j < uniformBlockName.size(); ++j){
		uniformBlockIndex[j] = glGetUniformBlockIndex(prog, uniformBlockName[j].c_str());
		if(uniformBlockIndex[j] == GL_INVALID_INDEX)
			std::cerr << "ERROR::Uniformblock name is not have a valid index!" << std::endl;
		else{
			uniformBlockSize[j] = -1;
			glGetActiveUniformBlockiv(prog, uniformBlockIndex[j], GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize[j]);
			if(uniformBlockSize[j] == -1)
				std::cerr << "ERROR::An error occurred getting the minimum size for the buffer!" << std::endl;
			else{
				switch(j){
					case 0:
						if(pointLightArray != NULL){
							unsigned char* pointLightBuffer = new unsigned char[uniformBlockSize[j]];
							uniformBlockMemberIndex.resize(uniformBlockCountMembers[j]);
							glGetUniformIndices(prog, uniformBlockCountMembers[j], pointLightMemberName, &uniformBlockMemberIndex[0]);
							for(uint index = 0; index < uniformBlockCountMembers[j]; ++index){
								switch(uniformBlockMemberIndex[index]){
									case GL_INVALID_INDEX:
										std::cerr << pointLightMemberName[index] << " is not a program variable shader or is not active!" << std::endl;
										exit(1);
										break;
								}
							}
							std::vector<int> offset(uniformBlockCountMembers[j]);
							glGetActiveUniformsiv(prog, uniformBlockCountMembers[j], &uniformBlockMemberIndex[0] , GL_UNIFORM_OFFSET, offset.data());
							for(uint i = 0; i < pointLightArray->size(); ++i){
								std::memcpy(pointLightBuffer + offset[0] + (sizeof(vec4<float>) * i), &(*pointLightArray)[i].position, sizeof(vec4<float>));
								std::memcpy(pointLightBuffer + offset[1] + (sizeof(vec4<float>) * i), &(*pointLightArray)[i].color, sizeof(vec4<float>));
							}
							// SRW::gUBO.push_back(0);
							glGenBuffers(1, &SRW::gUBO[j]);
							glBindBufferBase(GL_UNIFORM_BUFFER, j, SRW::gUBO[j]);
							glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize[j], pointLightBuffer, GL_DYNAMIC_DRAW);
							if(false){
								float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY));
								if(ptr != NULL){
									for(uint i = 0; i < uniformBlockSize[j] / sizeof(float); i += 4){
										vec4<float>(ptr[i], ptr[(i + 1)], ptr[(i + 2)], ptr[(i + 3)]).show("point");
									}
								}
								glUnmapBuffer(GL_UNIFORM_BUFFER);
							}
							glBindBuffer(GL_UNIFORM_BUFFER, 0);
							delete[] pointLightBuffer;
						}
					break;
					case 1:
						if(spotLightArray != NULL){
							unsigned char* spotLightBuffer = new unsigned char[uniformBlockSize[j]];
							uniformBlockMemberIndex.resize(uniformBlockCountMembers[j]);
							glGetUniformIndices(prog, uniformBlockCountMembers[j], spotLightMemberName, &uniformBlockMemberIndex[0]);
							for(uint index = 0; index < uniformBlockCountMembers[j]; ++index){
								switch(uniformBlockMemberIndex[index]){
									case GL_INVALID_INDEX:
										std::cerr << spotLightMemberName[index] << " is not a program variable shader or is not active!" << std::endl;
										exit(1);
										break;
								}
							}
							std::vector<int> offset(uniformBlockCountMembers[j]);
							glGetActiveUniformsiv(prog, uniformBlockCountMembers[j], &uniformBlockMemberIndex[0] , GL_UNIFORM_OFFSET, offset.data());
							for(uint i = 0; i < spotLightArray->size(); ++i){
								std::memcpy(spotLightBuffer + offset[0] + (sizeof(vec4<float>) * i), &(*spotLightArray)[i].direction, sizeof(vec4<float>));
								std::memcpy(spotLightBuffer + offset[1] + (sizeof(vec4<float>) * i), &(*spotLightArray)[i].position, sizeof(vec4<float>));
								std::memcpy(spotLightBuffer + offset[2] + (sizeof(vec4<float>) * i), &(*spotLightArray)[i].color, sizeof(vec4<float>));
							}
							// SRW::gUBO.push_back(0);
							glGenBuffers(1, &SRW::gUBO[j]);
							glBindBufferBase(GL_UNIFORM_BUFFER, j, SRW::gUBO[j]);
							glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize[j], spotLightBuffer, GL_DYNAMIC_DRAW);
							if(false){
								float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY));
								if(ptr != NULL){
									for(uint i = 0; i < uniformBlockSize[j] / sizeof(float); i += 4){
										vec4<float>(ptr[i], ptr[(i + 1)], ptr[(i + 2)], ptr[(i + 3)]).show("spot");
									}
								}
								glUnmapBuffer(GL_UNIFORM_BUFFER);
							}
							glBindBuffer(GL_UNIFORM_BUFFER, 0);
							delete[] spotLightBuffer;
						}
					break;
					case 2:
						if(sun != NULL){
							unsigned char* sunLightBuffer = new unsigned char[uniformBlockSize[j]];
							uniformBlockMemberIndex.resize(uniformBlockCountMembers[j]);
							glGetUniformIndices(prog, uniformBlockCountMembers[j], sunLightMemberName, &uniformBlockMemberIndex[0]);
							for(uint index = 0; index < uniformBlockCountMembers[j]; ++index){
								switch(uniformBlockMemberIndex[index]){
									case GL_INVALID_INDEX:
										std::cerr << sunLightMemberName[index] << " is not a program variable shader or is not active!" << std::endl;
										exit(1);
										break;
								}
							}
							std::vector<int> offset(uniformBlockCountMembers[j]);
							glGetActiveUniformsiv(prog, uniformBlockCountMembers[j], &uniformBlockMemberIndex[0] , GL_UNIFORM_OFFSET, offset.data());
							std::memcpy(sunLightBuffer + offset[0] + sizeof(vec4<float>), &sun->direction, sizeof(vec4<float>));
							std::memcpy(sunLightBuffer + offset[1] + sizeof(vec4<float>), &sun->color, sizeof(vec4<float>));
							// SRW::gUBO.push_back(0);
							glGenBuffers(1, &SRW::gUBO[j]);
							glBindBufferBase(GL_UNIFORM_BUFFER, j, SRW::gUBO[j]);
							glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize[j], sun, GL_DYNAMIC_DRAW);
							if(false){
								float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY));
								if(ptr != NULL){
									for(uint i = 0; i < uniformBlockSize[j] / sizeof(float); i += 4){
										vec4<float>(ptr[i], ptr[(i + 1)], ptr[(i + 2)], ptr[(i + 3)]).show("sun");
									}
								}
								glUnmapBuffer(GL_UNIFORM_BUFFER);
							}
							glBindBuffer(GL_UNIFORM_BUFFER, 0);
							delete[] sunLightBuffer;
						}
					break;
				}
			}
		}
	}
}

void SRW::sharedUniforms(mat4<float>& matrix, vec3<float>& cameraPos, Sun* sun, std::vector<PointLight>* pointLightArray, std::vector<SpotLight>* spotLightArray){
	sharedUniforms(sun, pointLightArray, spotLightArray);
	sharedUniforms(matrix, cameraPos, true);
}

//SECTION: Constructors and destructor
SRW::SRW(){
	//Nothing to do
}

SRW::~SRW(){
	glDeleteProgram(program);
	if(!sampler2D.empty()){
		for(uint i = 0; i < sampler2D.size(); ++i){
			glDeleteTextures(1, &sampler2D[i].texID);
		}
	}
	if(!samplerCubemap.empty()){
		for(uint i = 0; i < samplerCubemap.size(); ++i){
			glDeleteTextures(1, &samplerCubemap[i].texID);
		}
	}
	sampler2D.clear();
	samplerCubemap.clear();
	unif1b.clear();
	unif1f.clear();
	unif3f.clear();
	unifMat4f.clear();
}

void SRW::initUnifSampler2D(){
	if(!sampler2D.empty()){
		for(uint i = 0; i < sampler2D.size(); ++i){
			glActiveTexture(GL_TEXTURE0 + sampler2D[i].value);
			glBindTexture(GL_TEXTURE_2D, sampler2D[i].texID);
			glUniform1i(glGetUniformLocation(program, sampler2D[i].label.c_str()), sampler2D[i].value);
		}
	}
}
void SRW::initUnifSamplerCubemap(){
	if(!samplerCubemap.empty()){
		for(uint i = 0; i < samplerCubemap.size(); ++i){
			glActiveTexture(GL_TEXTURE0 + samplerCubemap[i].value);
			glBindTexture(GL_TEXTURE_CUBE_MAP, samplerCubemap[i].texID);
			glUniform1i(glGetUniformLocation(program, samplerCubemap[i].label.c_str()), samplerCubemap[i].value);
		}
	}
}

void SRW::initUnif1b(){
	if(!unif1b.empty()){
		for(uint i = 0; i < unif1b.size(); ++i){
			glUniform1i(unif1b[i].location, unif1b[i].value);
		}
	}
}

void SRW::initUnif1f(){
	if(!unif1f.empty()){
		for(uint i = 0; i < unif1f.size(); ++i){
			if(unif1f[i].ptrValue == NULL)
				glUniform1f(unif1f[i].location, unif1f[i].value);
			else
				glUniform1f(unif1f[i].location, *unif1f[i].ptrValue);				
		}
	}
}

void SRW::initUnif3f(){
	if(!unif3f.empty()){
		for(uint i = 0; i < unif3f.size(); ++i){
			if(unif3f[i].ptrValue == NULL)
				glUniform3fv(unif3f[i].location, 1, unif3f[i].value.address());
			else
				glUniform3fv(unif3f[i].location, 1, unif3f[i].ptrValue->address());
		}
	}
}

void SRW::initUnifMat4f(){
	if(!unifMat4f.empty()){
		for(uint i = 0; i < unifMat4f.size(); ++i){
			glUniformMatrix4fv(unifMat4f[i].location, 1, GL_FALSE, &unifMat4f[i].value->matrix[0][0]);
		}
	}
}

//SECTION: Setters to uniform instaces
void SRW::setUniform1b(std::string name, bool value){
	std::vector<uniform1b>::const_iterator it = unif1b.begin();
	int index = searchUniform1b(name);
	if(index != -1)
		unif1b.erase(it + index);
	unif1b.push_back(uniform1b(name, value));
	unif1b.back().location = glGetUniformLocation(program, unif1b.back().label.c_str());
	if(unif1b.back().location == -1){
		std::cout << unif1b.back().label << " is not a uniform variable or is not active!" << std::endl;
	}
}

void SRW::setUniform1f(std::string name, float value){
	std::vector<uniform1f>::const_iterator it = unif1f.begin();
	int index = searchUniform1f(name);
	if(index != -1)
		unif1f.erase(it + index);
	unif1f.push_back(uniform1f(name, value));
	unif1f.back().location = glGetUniformLocation(program, unif1f.back().label.c_str());
	if(unif1f.back().location == -1){
		std::cout << unif1f.back().label << " is not a uniform variable or is not active!" << std::endl;
	}
}

void SRW::setUniform1f(std::string name, float* value){
	std::vector<uniform1f>::const_iterator it = unif1f.begin();
	int index = searchUniform1f(name);
	if(index != -1)
		unif1f.erase(it + index);
	unif1f.push_back(uniform1f(name, value));
	unif1f.back().location = glGetUniformLocation(program, unif1f.back().label.c_str());
	if(unif1f.back().location == -1){
		std::cout << unif1f.back().label << " is not a uniform variable or is not active!" << std::endl;
	}
}

void SRW::setUniform3f(std::string name, vec3f value){
	std::vector<uniform3f>::const_iterator it = unif3f.begin();
	int index = searchUniform3f(name);
	if(index != -1)
		unif3f.erase(it + index);
	unif3f.push_back(uniform3f(name, value));
	unif3f.back().location = glGetUniformLocation(program, unif3f.back().label.c_str());
	if(unif3f.back().location == -1){
		std::cout << unif3f.back().label << " is not a uniform variable or is not active!" << std::endl;
	}
}

void SRW::setUniform3f(std::string name, vec3f* value){
	std::vector<uniform3f>::const_iterator it = unif3f.begin();
	int index = searchUniform3f(name);
	if(index != -1)
		unif3f.erase(it + index);
	unif3f.push_back(uniform3f(name, value));
	unif3f.back().location = glGetUniformLocation(program, unif3f.back().label.c_str());
	if(unif3f.back().location == -1){
		std::cout << unif3f.back().label << " is not a uniform variable or is not active!" << std::endl;
	}
}

void SRW::setUniformMat4f(std::string name, mat4<float>* value){
	std::vector<uniformMat4f>::const_iterator it = unifMat4f.begin();
	int index = searchUniformMat4f(name);
	if(index != -1)
		unifMat4f.erase(it + index);
	unifMat4f.push_back(uniformMat4f(name, value));
	unifMat4f.back().location = glGetUniformLocation(program, unifMat4f.back().label.c_str());
	if(unifMat4f.back().location == -1){
		std::cout << unifMat4f.back().label << " is not a uniform variable or is not active!" << std::endl;
	}
}

void SRW::addTexture(const char* texPath, const char* uniformName, uint texUnit, uint profile, uint* parmWidth, uint* parmHeight){
	int index = searchUniformSampler2D(std::string(uniformName));
	if(index != -1){
		// std::cerr << "Uniform name has defined!\n";
		std::vector<uniform1i>::iterator it = sampler2D.begin() + index;
		sampler2D.erase(it);
	}
	uint id      = 0;
	int width    = 0;
	int height   = 0;
	int channels = 0;
	uchar* data	= NULL; 
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	if(!glIsTexture(id)) exit(0);
	data = stbi_load(texPath, &width, &height, &channels, 0);
	if(parmWidth != NULL && parmHeight != NULL){
		*parmHeight = height;
		*parmWidth  = width;
	}
	channels = channels == 4? channels + 1: channels == 3? channels + 1: channels == 2? channels - 1: 0; 
	if(data){
		switch(profile){
			case SRGB_PROFILE:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);			
				break;
			case SRGBA_PROFILE:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);			
				break;
			case DEFAULT_PROFILE:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED + channels, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);
				break;
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else{
		std::cerr << "srw::ERROR::Failed to load texture: " << texPath << std::endl;
		std::cerr << "srw::WARNING::Exiting..." << std::endl;		
		exit(1);
	}
	stbi_image_free(data);	
	sampler2D.push_back(uniform1i(uniformName, texUnit, id));
}
void SRW::addTexture(uint& id, const char* uniformName, uint texUnit, uint uniformType){
	if(uniformType){
		samplerCubemap.push_back(uniform1i(uniformName, texUnit, id));
	}
	else{
		sampler2D.push_back(uniform1i(uniformName, texUnit, id));
	}
}

void SRW::setProgram(uint& prog){
	program = prog;
}

//SECTION: Getters
uint& SRW::getProgram(){
	return program;
}

SRW::uniform1b& SRW::getUniform1b(uint index){
	return unif1b[index];
}

SRW::uniform1f& SRW::getUniform1f(uint index){
	return unif1f[index];
}

SRW::uniform3f& SRW::getUniform3f(uint index){
	return unif3f[index];
}

SRW::uniformMat4f& SRW::getUniformMat4f(uint index){
	return unifMat4f[index];
}

//Search by name in the uniform vectors
int SRW::searchUniformSampler2D(std::string name){
	uint i;
	for(i = 0; i < sampler2D.size(); ++i){
		if(name.compare(sampler2D[i].label) != 0)
			continue;
		else
			break;
	}
	if(i < sampler2D.size()){
		if(name.compare(sampler2D[i].label) == 0){
			return i;
		}
	}
	std::cout << name << " in std::vector sampler2D is not found!" << std::endl;
	return -1;
}

int SRW::searchUniform1b(std::string name){
	uint i;
	for(i = 0; i < unif1b.size(); ++i){
		if(name.compare(unif1b[i].label) != 0)
			continue;
		else
			break;
	}
	if(i < unif1b.size()){
		if(name.compare(unif1b[i].label) == 0){
			return i;
		}
	}
	std::cout << name << " in std::vector unif1b is not found!" << std::endl;
	return -1;
}

int SRW::searchUniform1f(std::string name){
	uint i;
	for(i = 0; i < unif1f.size(); ++i){
		if(name.compare(unif1f[i].label) != 0)
			continue;
		else
			break;
	}
	if(i < unif1f.size()){
		if(name.compare(unif1f[i].label) == 0){
			return i;
		}
	}
	std::cout << name << " in std::vector unif1f is not found!" << std::endl;
	return -1;
}

int SRW::searchUniform3f(std::string name){
	uint i;
	for(i = 0; i < unif3f.size(); ++i){
		if(name.compare(unif3f[i].label) != 0)
			continue;
		else
			break;
	}
	if(i < unif3f.size()){
		if(name.compare(unif3f[i].label) == 0){
			return i;
		}
	}
	std::cout << name << " in std::vector unif3f is not found!" << std::endl;
	return -1;
}

int SRW::searchUniformMat4f(std::string name){
	uint i;
	for(i = 0; i < unifMat4f.size(); ++i){
		if(name.compare(unifMat4f[i].label) != 0)
			continue;
		else
			break;
	}
	if(i < unifMat4f.size()){
		if(name.compare(unifMat4f[i].label) == 0){
			return i;
		}
	}
	std::cout << name << " in std::vector unifMat4f is not found!" << std::endl;
	return -1;
}

//SECTION: Instance information
void SRW::info(){
	std::cout << std::setw(12) <<"Program ID: " << program << "\n" << std::endl;
	if(!sampler2D.empty()){
		std::cout << std::setw(12) << "\nSampler2D:\n" << std::endl;
		for(uint i = 0; i < sampler2D.size(); ++i){
			std::cout << "[" << sampler2D[i].texID << "]" << std::endl;
		}
	}
	if(!samplerCubemap.empty()){
		std::cout << std::setw(12) << "\nSamplerCubemap:\n" << std::endl;
		for(uint i = 0; i < samplerCubemap.size(); ++i){
			std::cout << std::setw(20) << samplerCubemap[i].label << " [" << samplerCubemap[i].value << "]" << std::endl;
		}
	}
	if(!unif1b.empty()){
		std::cout << std::setw(12) << "\nUniform1b:\n" << std::endl;
		for(uint i = 0; i < unif1b.size(); ++i){
			std::cout << std::setw(20) << unif1b[i].label << " [" << unif1b[i].value << "]" << std::endl;
		}
	}
	if(!unif1f.empty()){
		std::cout << std::setw(12) << "\nUniform1f:\n" << std::endl;
		for(uint i = 0; i < unif1f.size(); ++i){
			if(unif1f[i].ptrValue == NULL)
				std::cout << std::setw(20) << unif1f[i].label << " [" << unif1f[i].value << "]" << std::endl;
			else{
				std::cout << std::setw(20) << unif1f[i].label << " [" << *unif1f[i].ptrValue << "]" << std::endl;
			}
		}
	}
	if(!unif3f.empty()){
		std::cout << std::setw(12) << "\nUniform3f:\n" << std::endl;
		for(uint i = 0; i < unif3f.size(); ++i){
			unif3f[i].value.show(unif3f[i].label);	
		}
	}
	if(!unifMat4f.empty()){
		std::cout << std::setw(12) << "\nMatrices:\n" << std::endl;
		for(uint i = 0; i < unifMat4f.size(); ++i){
			unifMat4f[i].value->show(unifMat4f[i].label);
		}
	}
}