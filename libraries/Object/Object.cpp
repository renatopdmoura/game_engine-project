/*** 
 * Copyright, 16/11/2021
 * Novos recursos:
 * 1 - Limitar a quantidade de ids de textura para não exceder
	a quantia efetiva por objeto.
 * 2 - Permitir de forma coesa que objetos usem diferentes paradigmas de renderização.
 * 3 - Tornar a escrita em shaders dinâmica.
***/

#include "Object.hpp"

std::vector<Object*> Object::stack;

#if RENDER_DEBUG_MODE
	void Object::renderDebugDepth(mat4<float>& viewProj, float zNear, float zFar){
		for(uint i = 0; i < Object::stack.size(); ++i){
			Object::stack[i]->setProgram(SRW::debugPrograms[0]);
			Object::stack[i]->setUniform1f("zNear", zNear);
			Object::stack[i]->setUniform1f("zFar", zFar);
			Object::stack[i]->setUniformMat4f("viewProj", &viewProj);
			Object::stack[i]->setUniformMat4f("model", &Object::stack[i]->getModel());
		}
	}

	void Object::renderDebugNormal(mat4<float>& view, mat4<float>& projection){
		for(uint i = 0; i < Object::stack.size(); ++i){
			Object::stack[i]->setProgram(SRW::debugPrograms[1]);
			Object::stack[i]->setUniformMat4f("view", &view);
			Object::stack[i]->setUniformMat4f("projection", &projection);
			Object::stack[i]->setUniformMat4f("model", &Object::stack[i]->getModel());
		}
	}
#endif

// - Verifica se todos os dados para o sombreador pbr foram definidos
void Object::completeness(){
	bool success = true;
	if(!stack.empty()){
		for(uint i = 0; i < stack.size(); ++i){
			std::cout  << std::endl << "Object name: " << stack[i]->getName() << std::endl;
			if(stack[i]->searchUniform3f("material.albedo") == -1 && stack[i]->searchUniformSampler2D("material.albedo") == -1){
				success = false;
			}
			if(stack[i]->searchUniformSampler2D("material.normal") == -1){
				std::cerr << "Using normal surface." << std::endl;
			}
			if(stack[i]->searchUniform1f("material.roughness") == -1 && stack[i]->searchUniformSampler2D("material.roughness") == -1){
				success = false;
			}
			if(stack[i]->searchUniform1f("material.metallic") == -1 && stack[i]->searchUniformSampler2D("material.metallic") == -1){
				success = false;
			}
			if(stack[i]->searchUniform1f("material.ao") == -1 && stack[i]->searchUniformSampler2D("material.ao") == -1){
				success = false;
			}
		}
	}
	if(!success){ 
		std::cerr << "For some object, there are undefined textures.\nApplication closed with failure." << std::endl;
		exit(1);
	}
}

// - Para programas de sombreameto exlcusivos
Object::Object(std::string model_path, std::string vs_path, std::string fs_path){
	Object::stack.push_back(this);
	if(parser(model_path)){
		genShader(vs_path, fs_path);
		genBuffers();
		setUniformMat4f("model", &model);
	}
}

// - Construtor usual tendo um parâmetro que indica a quantia de instâncias
Object::Object(std::string model_path, uint type, std::vector<mat4<float>>* matrices, uint method){
	Object::stack.push_back(this);
	shaderType = type;
	if(parser(model_path)){
		if(matrices != NULL){
			instantiate(matrices, method);
		}
		else{
			switch(shaderType){
				case UNIFORM_COLOR:
					setProgram(SRW::programs[UNIFORM_COLOR]);
					break;
				case TEXTURIZED:
					setProgram(SRW::programs[TEXTURIZED]);
					break;
			}
			setUniformMat4f("model", &model);
		}
		genBuffers();
	}
}

Object::~Object(){
	free();
}

void Object::free(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &UBO);
	vertex.clear();
	arrayModel = NULL;
}

void Object::genBuffers(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), &vertex[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // texcoord
	glEnableVertexAttribArray(3); // tangente
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(sizeof(float) * 8));
	glBindVertexArray(0);
}

void Object::render(){
	glBindVertexArray(VAO);
	for(uint i = 0; i < SRW::gUBO.size(); i++){
		if(glIsBuffer(SRW::gUBO[i]))
			glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
	}
	initUnifSampler2D();
	initUnif1f();
	initUnif3f();
	initUnifMat4f();
	// glUniformMatrix4fv(glGetUniformLocation(program, "normalSpace"), 1, GL_FALSE, &((model.inverse()).transpose())[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 11);
	glBindVertexArray(0);
}

void Object::render(uint instances){
	glUseProgram(program);
	glBindVertexArray(VAO);
	for(uint i = 0; i < SRW::gUBO.size(); i++){
		if(glIsBuffer(SRW::gUBO[i]))
			glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
	}
	if(instanceMethod == UNIFORM_ARRAY){
		for(uint i = 0; i < instances; ++i){
			glUniformMatrix4fv(glGetUniformLocation(program, std::string("arrayModels[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &(*arrayModel)[i][0][0]);
			// glUniformMatrix4fv(glGetUniformLocation(program, std::string("normalSpace[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &(*arrayModel)[i].inverse().transpose()[0][0]);
		}
	}
	else
		glBindBufferBase(GL_UNIFORM_BUFFER, 4, UBO);
	initUnifSampler2D();
	initUnif1f();
	initUnif3f();
	initUnifMat4f();
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertex.size() / 11, instances);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Object::instantiate(std::vector<mat4<float>>* matrices, uint method){
	arrayModel = matrices;
	std::string strVertexShader;
	// - Uniform array method
	instanceMethod = method;
	if(method == UNIFORM_ARRAY){
		std::vector<std::string> source = {
				"#version 460 core\n",
				"layout(location = 0) in vec4 position;\n",
				"layout(location = 1) in vec3 normal;\n",
				"layout(location = 2) in vec2 texCoords;\n",
				"layout(location = 3) in vec3 tangent;\n",
				"out struct VS_OUT{\n",
					"vec3 fragWorldPos;\n",
					"vec3 cameraPos;\n",
					"vec2 texCoords;\n",
					"mat3 TBN;\n",
					"vec3 normal;\n",
				"}vs_out;\n",
				"layout(binding = 3, shared) uniform Var{\n",
					"mat4 viewProj;\n",
					"vec4 cameraPos;\n",
				"};\n",
				"uniform mat4 arrayModels[" + std::to_string(arrayModel->size()) + "];\n",
				"uniform mat4 normalSpace[" + std::to_string(arrayModel->size()) + "];\n",
				"void main(){\n",
					"//mat3 NS = mat3(normalSpace[gl_InstanceID]);\n",
					"mat3 NS = mat3(transpose(inverse(model[gl_InstanceID])));\n",
					"vec3 T  = normalize(NS * tangent);\n",
					"vec3 N  = normalize(NS * normal);\n",
					"T       = normalize(T - dot(T, N) * N);\n",
					"vec3 B  = NS * cross(N, T);\n",
					"vs_out.TBN          = mat3(T, B, N);\n",
					"vs_out.normal 		 = NS * normal;\n",
					"vs_out.fragWorldPos = vec3(arrayModels[gl_InstanceID] * position);\n",
					"vs_out.cameraPos    = vec3(cameraPos);\n",
					"vs_out.texCoords    = texCoords;\n",
					"gl_Position         = viewProj * arrayModels[gl_InstanceID] * position;\n",
				"}"
		};
		for(uint i = 0; i < source.size(); ++i){
			strVertexShader += source[i];
		}
		createProgram(program);
		createShader(program, GL_VERTEX_SHADER, strVertexShader);
		switch(shaderType){
			case UNIFORM_COLOR:
				createShader(program, GL_FRAGMENT_SHADER, readShaderSource("../shaders/object/fs_uniform_color.glsl"));
				break;
			case TEXTURIZED:
				createShader(program, GL_FRAGMENT_SHADER, readShaderSource("../shaders/object/fs_textured.glsl"));
				break;
		}
		linkProgram(program);
	}
	// - Uniform block method
	else if(method == UNIFORM_BLOCK){
		std::vector<std::string> source = {
				"#version 460 core\n",
				"layout(location = 0) in vec4 position;\n",
				"layout(location = 1) in vec3 normal;\n",
				"layout(location = 2) in vec2 texCoords;\n",
				"layout(location = 3) in vec3 tangent;\n",
				"out struct VS_OUT{\n",
					"vec3 fragWorldPos;\n",
					"vec3 cameraPos;\n",
					"vec2 texCoords;\n",
					"mat3 TBN;\n",
					"vec3 normal;\n",
				"}vs_out;\n",
				"layout(binding = 3, shared) uniform Var{\n",
					"mat4 viewProj;\n",
					"vec4 cameraPos;\n",
				"};\n",
				"layout(binding = 4) uniform Object{\n",
					"mat4 model[" + std::to_string(arrayModel->size()) + "];\n",
					"mat4 normalSpace[" + std::to_string(arrayModel->size()) + "];\n",
				"};\n",
				"void main(){\n",
					"//mat3 NS = mat3(normalSpace[gl_InstanceID]);\n",
					"mat3 NS = mat3(transpose(inverse(model[gl_InstanceID])));\n",
					"vec3 T  = normalize(NS * tangent);\n",
					"vec3 N  = normalize(NS * normal);\n",
					"T       = normalize(T - dot(T, N) * N);\n",
					"vec3 B  = NS * cross(N, T);\n",
					"vs_out.TBN          = mat3(T, B, N);\n",
					"vs_out.normal 		 = NS * normal;\n",
					"vs_out.fragWorldPos = vec3(model[gl_InstanceID] * position);\n",
					"vs_out.cameraPos    = vec3(cameraPos);\n",
					"vs_out.texCoords    = texCoords;\n",
					"gl_Position         = viewProj * model[gl_InstanceID] * position;\n",
				"}"
		};
		for(uint i = 0; i < source.size(); ++i){
			strVertexShader += source[i];
		}
		createProgram(program);
		createShader(program, GL_VERTEX_SHADER, strVertexShader);
		switch(shaderType){
			case UNIFORM_COLOR:
				createShader(program, GL_FRAGMENT_SHADER, readShaderSource("../shaders/object/fs_uniform_color.glsl"));
				break;
			case TEXTURIZED:
				createShader(program, GL_FRAGMENT_SHADER, readShaderSource("../shaders/object/fs_textured.glsl"));
				break;
		}
		linkProgram(program);
		uint blockIndex = glGetUniformBlockIndex(program, "Object");
		if(blockIndex == GL_INVALID_INDEX)
			std::cerr << "ERROR::Uniformblock name is not have a valid index!" << std::endl;
		else{
			int blockSize = -1;
			glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			if(blockSize == -1)
				std::cerr << "ERROR::An error occurred getting the minimum size for the buffer!" << std::endl;
			else{
				unsigned char* blockBuffer = new unsigned char[blockSize];
				uint countMembers = 2;
				const char* names[] = {"model", "normalSpace"};
				uint indices[countMembers];
				glGetUniformIndices(program, countMembers, names, indices);
				bool success = true;
				for(uint index = 0; index < countMembers; ++index){
					switch(indices[index]){
						case GL_INVALID_INDEX:
							std::cerr << names[index] << " is not a program variable shader or is not active!" << std::endl;
							success = false;
							break;
					}
				}
				if(success){
					std::vector<int> offset(countMembers);
					glGetActiveUniformsiv(program, countMembers, indices, GL_UNIFORM_OFFSET, offset.data());
					for(uint i = 0; i < arrayModel->size(); ++i){
						std::memcpy(blockBuffer + offset[0] + (sizeof(mat4<float>) * i), &(*arrayModel)[i][0][0], sizeof(mat4<float>));
						std::memcpy(blockBuffer + offset[1] + (sizeof(mat4<float>) * i), &(*arrayModel)[i].inverse().transpose()[0][0], sizeof(mat4<float>));
					}
					glGenBuffers(1, &UBO);
					glBindBufferBase(GL_UNIFORM_BUFFER, 4, UBO);
					glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}
			}
		}	
	}
}

void Object::updateInstances(){
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, UBO);
	int blockSize = arrayModel->size() * sizeof(mat4<float>) * 2;
	for(uint i = 0; i < arrayModel->size(); ++i){
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(mat4<float>), sizeof(mat4<float>), &(*arrayModel)[i][0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, (i * sizeof(mat4<float>)) + (blockSize / 2), sizeof(mat4<float>), &(*arrayModel)[i].inverse().transpose()[0][0]);

	}	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

inline void Object::search(std::vector<float>* container, std::ifstream* file, char code){
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

bool Object::parser(std::string model_path){
	std::ifstream file(model_path.c_str(), std::ios_base::in);
	if(!file.is_open()){
		std::cout << "Failed to open file " << model_path << std::endl;
		return false;
	}
	else{
		std::string strData;
		char code     = '\0';
		bool readLine = false;
		bool indexv   = false;
		bool indexn   = false;
		bool indext   = false;
		unsigned int slash  = 0;
		//buffers brutos para armazenamento
		std::vector<float> position;
		std::vector<float> normal;
		std::vector<float> texture;
		//buffers de índices
		std::vector<int> indexVertexStream;
		std::vector<int> indexNormalStream;
		std::vector<int> indexTextureStream;
		std::vector<vec3f> stream;
		//buffers que auxiliam na manipulação de dodos
		std::vector<vec3f> pos;
		std::vector<vec3f> norm;
		std::vector<vec2f> tex;
		while(file.get(code)){
			switch(code){
				//Obtém o nome do objeto 
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
				//Salva os atributos nos respectivos buffers
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
				//Obtém os indíces dos atributos de vértices
				case 'f':
					file.get(code);
					if(code == ' '){
						readLine = true;
						while(readLine){
							if(code == ' '){
								slash  = 0;
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
		//Arranja os dados de um vetor linear para um tipo vec3f e vec2f
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
			vec2f temp;
			temp.x = texture[k];
			temp.y = texture[k + 1];
			tex.push_back(temp);
			k += 2;
		}

		uint tot   = indexVertexStream.size() + indexNormalStream.size() + indexTextureStream.size();
		uint index = 0, iv = 0, in = 0, it = 0;
		std::vector<vertexAttrib> v_buffer;
		vertexAttrib temp;
		while(index < tot){
			if(iv < indexVertexStream.size()){
				temp.position = pos[indexVertexStream[iv]];
				iv++;
				index++;
			}
			if(in < indexNormalStream.size()){
				temp.normal = norm[indexNormalStream[in]];
				in++;
				index++;
			}
			if(it < indexTextureStream.size()){
				temp.texcoord = tex[indexTextureStream[it]];
				it++;
				index++;
			}
			v_buffer.push_back(temp);
		}
		vec3<float> p1, p2, p3;
		vec2<float> uv1, uv2, uv3;
		vec3<float> edge1, edge2;
		vec2<float> deltaUV1, deltaUV2;
		for(uint i = 0; i < v_buffer.size(); i += 3){
			p1    = v_buffer[i].position;
			p2    = v_buffer[i + 1].position;
			p3    = v_buffer[i + 2].position;
			uv1   = v_buffer[i].texcoord;
			uv2   = v_buffer[i + 1].texcoord;
			uv3   = v_buffer[i + 2].texcoord;
			edge1 = p2 - p1;
			edge2 = p3 - p1;
			deltaUV1 = uv2 - uv1;
			deltaUV2 = uv3 - uv1;
			float f  = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y)? 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y): 0.0f;
			vec3<float> tang;
			tang.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tang.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tang.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			
			v_buffer[i].tangent     = tang;		 
			v_buffer[i + 1].tangent = tang;		 
			v_buffer[i + 2].tangent = tang;

			vertex.push_back(v_buffer[i].position.x);
			vertex.push_back(v_buffer[i].position.y);
			vertex.push_back(v_buffer[i].position.z);
			vertex.push_back(v_buffer[i].normal.x);
			vertex.push_back(v_buffer[i].normal.y);
			vertex.push_back(v_buffer[i].normal.z);
			vertex.push_back(v_buffer[i].texcoord.x);
			vertex.push_back(v_buffer[i].texcoord.y);
			vertex.push_back(v_buffer[i].tangent.x);
			vertex.push_back(v_buffer[i].tangent.y);
			vertex.push_back(v_buffer[i].tangent.z);

			vertex.push_back(v_buffer[i + 1].position.x);
			vertex.push_back(v_buffer[i + 1].position.y);
			vertex.push_back(v_buffer[i + 1].position.z);
			vertex.push_back(v_buffer[i + 1].normal.x);
			vertex.push_back(v_buffer[i + 1].normal.y);
			vertex.push_back(v_buffer[i + 1].normal.z);
			vertex.push_back(v_buffer[i + 1].texcoord.x);
			vertex.push_back(v_buffer[i + 1].texcoord.y);
			vertex.push_back(v_buffer[i + 1].tangent.x);
			vertex.push_back(v_buffer[i + 1].tangent.y);
			vertex.push_back(v_buffer[i + 1].tangent.z);

			vertex.push_back(v_buffer[i + 2].position.x);
			vertex.push_back(v_buffer[i + 2].position.y);
			vertex.push_back(v_buffer[i + 2].position.z);
			vertex.push_back(v_buffer[i + 2].normal.x);
			vertex.push_back(v_buffer[i + 2].normal.y);
			vertex.push_back(v_buffer[i + 2].normal.z);
			vertex.push_back(v_buffer[i + 2].texcoord.x);
			vertex.push_back(v_buffer[i + 2].texcoord.y);
			vertex.push_back(v_buffer[i + 2].tangent.x);
			vertex.push_back(v_buffer[i + 2].tangent.y);
			vertex.push_back(v_buffer[i + 2].tangent.z);
		}
	}
	return true;
}

std::string Object::getName(){
	return name;
}

mat4<float>& Object::getModel(){
	return model; 
}

std::vector<float> Object::getVertexBufferArray(){
	return vertex;
}

uint Object::getVAO() const{
	return VAO;
}

uint Object::getVBO() const{
	return VBO;
}