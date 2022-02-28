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
float Object::forceG = 0.5f;

#if RENDER_DEBUG_MODE
void Object::renderDebugDepth(mat4<float>& viewProj, float zNear, float zFar){
	for(uint i = 0; i < Object::stack.size(); ++i){
		Object::stack[i]->setProgram(SRW::debugPrograms[0]);
		Object::stack[i]->setUniform1f("zNear", zNear);
		Object::stack[i]->setUniform1f("zFar", zFar);
		Object::stack[i]->setUniformMat4f("viewProj", &viewProj);
		Object::stack[i]->setUniformMat4f("debug_depth_model", &Object::stack[i]->getModel());
	}
}

void Object::renderDebugNormal(mat4<float>& view, mat4<float>& projection){
	for(uint i = 0; i < Object::stack.size(); ++i){
		Object::stack[i]->setProgram(SRW::debugPrograms[1]);
		Object::stack[i]->setUniformMat4f("view", &view);
		Object::stack[i]->setUniformMat4f("projection", &projection);
		Object::stack[i]->setUniformMat4f("debug_normal_model", &Object::stack[i]->getModel());
	}
}

// void Object::select(Camera& camera, SDL_Event* event, std::vector<Text>& objList){
// 	// - In use ray casting and sphere collision test
// 	Object* obj = NULL;
// 	if(event->type == SDL_MOUSEBUTTONDOWN){
// 		if(event->button.button == SDL_BUTTON_LEFT){
// 			for(std::vector<Object*>::iterator it = Object::stack.begin(); it != Object::stack.end(); ++it){
// 				// - Screen Coordinates to NDC[-1, 1]
// 				float x = 2.0f * (float)event->motion.x / (float)ext_screen_width - 1.0f;
// 				float y = 2.0f * (ext_screen_height - event->motion.y) / ext_screen_height - 1.0f; 						
// 				// - Get ray in world space		
// 				vec3<float> rayBegin = camera.getPosition();
// 				vec4<float> rayEnd   = vec4<float>(x, y, 1.0f, 1.0f) * inverse(camera.getProjectionMatrix()); // - Do perspective division
// 				rayEnd = mat4xvec4(inverse(camera.getViewMatrix().transpose()), vec4<float>(rayEnd.x, rayEnd.y, rayEnd.z, 1.0f));						
// 				// - Get sphere parameters and test collision
// 				vec3<float> centerSphere = vec3<float>((*it)->getModel()[3][0], (*it)->getModel()[3][1], (*it)->getModel()[3][2]);
// 				// - Get radius sphere by greatest absolute value from scale component
// 				float radius = 0.0f;
// 				if((*it)->getModel()[0][0] >= (*it)->getModel()[1][1] && (*it)->getModel()[1][1] >= (*it)->getModel()[2][2])
// 					radius = (*it)->getModel()[0][0];
// 				else if((*it)->getModel()[1][1] >= (*it)->getModel()[2][2])
// 					radius = (*it)->getModel()[1][1];
// 				else
// 					radius = (*it)->getModel()[2][2];
// 				// - Ensures that the ray is not launched from inside the sphere
// 				if(length(centerSphere, rayBegin) < radius){
// 					// std::cout << "Inside sphere!\n";
// 					continue;
// 				}
// 				vec3<float> dirToSphere  = centerSphere - rayBegin;
// 				vec3<float> dirLine      = normalize(rayEnd - rayBegin);
// 				float lenLine            = length(rayBegin, rayEnd);
// 				float t = dot(dirLine, dirToSphere);
// 				vec3<float> closestPoint;
// 				if(t <= 0.0f){
// 					closestPoint = rayBegin;
// 					if(t == 0.0f){
// 						// std::cout << "Tangent crossing!\n";
// 						continue;
// 					}
// 					else if(t < 0.0f){ 
// 						// std::cout << "Missing intersection!\n";
// 						continue;
// 					}
// 				}
// 				if(t >= lenLine){
// 					closestPoint = rayEnd;
// 					// std::cout << "Object is beyond depth selectable.\n";
// 					continue;
// 				}
// 				closestPoint = rayBegin + dirLine * t;
// 				if(length(centerSphere, closestPoint) <= radius){
// 					obj = *it;
// 				}
// 			}
// 		}
// 	}
// 	if(obj != NULL){
// 		for(uint i = 0; i < objList.size(); ++i){
// 			if(objList[i].getLabel() == obj->getName()){
// 				objList[i].setTextColor(vec3<float>(1.0f, 1.0f, 0.0f));
// 			}
// 			if(objList[i].getLabel() != obj->getName()){
// 				objList[i].setTextColor(vec3<float>(0.9f));
// 			}
// 		}
// 	}
// }
#endif

// void Object::renameIfExists(){
// 	std::string nameComparison = "UNKNOWN";
// 	uint nameRepetitions       = 0;
// 	for(std::vector<Object*>::iterator it = Object::stack.begin(); it != Object::stack.end(); ++it){
// 		nameRepetitions = 0;
// 		nameComparison  = (*it)->getName();
// 		for(std::vector<Object*>::iterator is = Object::stack.begin(); is < Object::stack.end(); ++is){
// 			if(nameComparison.compare((*is)->getName()) == 0){
// 				(*it)->setName(Text::uppercase((*it)->getName()) + (nameRepetitions > 0? std::to_string(nameRepetitions): ""));
// 				nameRepetitions++;					
// 			}
// 			else
// 				(*it)->setName(Text::uppercase((*it)->getName()));
// 		}
// 	}
// }

// - Verifica se todos os dados para o sombreador PBR foram definidos
void Object::completeness(){
	bool success = true;
	if(!stack.empty()){
		// Object::renameIfExists();
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

// - Para programas de sombreameto exclusivos
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
	// - Animated key frames
	else{
		std::cout << "Object::Info::The path is a possible directory! Searching for files..." << std::endl;
		enable_multiple_files = true;
		for(auto const& dir_entry : std::filesystem::directory_iterator{model_path}){
			parser(model_path + "/" + dir_entry.path().filename().string());
		}
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
	keyframeBuffer.clear();
	arrayModel = NULL;
}

void Object::genBuffers(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if(enable_multiple_files)
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * keyframeBuffer[0].size(), &keyframeBuffer[0][0], GL_DYNAMIC_DRAW);
	else
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
		if(enable_multiple_files){
			keyframeBuffer.push_back(vertex);
			vertex.clear();
		}
	}
	return true;
}

// - Rendering

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
	glUniformMatrix4fv(glGetUniformLocation(SRW::programs[TEXTURIZED], "normalSpace"), 1, GL_FALSE, &inverse(model).transpose()[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 11);
	glBindVertexArray(0);
}

void Object::render(uint instances, uint frame){
	glUseProgram(program);
	glBindVertexArray(VAO);
	for(uint i = 0; i < SRW::gUBO.size(); i++){
		if(glIsBuffer(SRW::gUBO[i]))
			glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
	}
	if(instanceMethod == UNIFORM_ARRAY){
		for(uint i = 0; i < instances; ++i){
			glUniformMatrix4fv(glGetUniformLocation(program, std::string("arrayModels[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &(*arrayModel)[i][0][0]);
			glUniformMatrix4fv(glGetUniformLocation(program, std::string("normalSpace[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &inverse((*arrayModel)[i])[0][0]);
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

void Object::render(uint frame, bool repeat, uint msBegin, uint msEnd){
	glBindVertexArray(VAO);
		for(uint i = 0; i < SRW::gUBO.size(); i++){
		if(glIsBuffer(SRW::gUBO[i]))
			glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
	}
	initUnifSampler2D();
	initUnif1f();
	initUnif3f();
	initUnifMat4f();
	glUniformMatrix4fv(glGetUniformLocation(program, "normalSpace"), 1, GL_FALSE, &inverse(model).transpose()[0][0]);
	
	if(repeat){
		next_tick += 16;
		if(next_tick <= msBegin + msEnd){
			if(next_tick >= msBegin){
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * keyframeBuffer[frame].size(), &(keyframeBuffer[frame])[0]);
			}
		}
		else 
			repeat = false;
	}
	else if(rewrite_buffer_once){
		rewrite_buffer_once = false;
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * keyframeBuffer[frame].size(), &(keyframeBuffer[frame])[0]);
	}
	else{
		next_tick = 0;
		rewrite_buffer_once = true;
	}
	
	glDrawArrays(GL_TRIANGLES, 0, keyframeBuffer[frame].size() / 11);
	glBindVertexArray(0);
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
				const uint countMembers = 2;
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
						// std::memcpy(blockBuffer + offset[1] + (sizeof(mat4<float>) * i), &(*arrayModel)[i].inverse().transpose()[0][0], sizeof(mat4<float>));
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
		// glBufferSubData(GL_UNIFORM_BUFFER, (i * sizeof(mat4<float>)) + (blockSize / 2), sizeof(mat4<float>), &(*arrayModel)[i]..transpose()[0][0]);

	}	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// - Transforming

void Object::pushMatrix(mat4<float> matrix){
	model.identity();
	transforms.push_back(matrix);
	for(uint i = 0; i < transforms.size(); ++i){
		model = model * transforms[i];
	}
}

void Object::updateModelMatrixPerFrame(mat4<float> matrix){
	model.identity();
	for(uint i = 0; i < transforms.size(); ++i){
		model = model * transforms[i];
	}
	model = model * matrix;
}

// - Physics

void Object::SpherexSphere(Camera& camera){
	// - Gravity action
	if(model[3][1] > model[1][1] + 0.1f) model[3][1] -= Object::forceG;
	if(model[3][1] < model[1][1] + 0.1f) model[3][1] += model[1][1] + 0.1f - model[3][1];

	// - Sphere Collision
	vec3<float> centerSphere = vec3<float>(getModel()[3][0], getModel()[3][1], getModel()[3][2]);
	float radiusSphere       = 1.0f; // - Definido para 1 temporariamente
	
	// - Ao realizar transformações de rotação, a diagonal principal não refletira mais a proporção de escala do objeto
	if(getModel()[0][0] >= getModel()[1][1] && getModel()[1][1] >= getModel()[2][2]){
		radiusSphere = getModel()[0][0];
	}
	else if(getModel()[1][1] >= getModel()[2][2]){
		radiusSphere = getModel()[1][1];
	}
	else{
		radiusSphere = getModel()[2][2];
	}
	float radiusCamera = 1.0f;
	float distance     = length(centerSphere, camera.getPosition());
	if(distance <= (radiusCamera + radiusSphere) + 0.5f){
		camera.setPosition(cameraBeforeCollision);
	}
	else{
		cameraBeforeCollision = camera.getPosition();
	}
}

void Object::AABBxAABB(Camera& camera){
	// - Gravity action
	if(model[3][1] > model[1][1] + 0.1f) model[3][1] -= Object::forceG;
	if(model[3][1] < model[1][1] + 0.1f) model[3][1] += model[1][1] + 0.1f - model[3][1];

	
	// - AABB Collision detecion
	vec3<float> centerCamera = camera.getPosition();
	vec3<float> centerObject = vec3<float>(getModel()[3][0], getModel()[3][1], getModel()[3][2]);
	
	// - Radius of the unitary cube
	float radiusCube = 1.0f;
	
	// - Boundary vertices
	vec3<float> A_min;
	vec3<float> A_max;
	vec3<float> B_min;
	vec3<float> B_max;

	A_min.x = centerCamera.x - radiusCube;
	A_max.x = centerCamera.x + radiusCube;
	A_min.y = centerCamera.y - radiusCube;
	A_max.y = centerCamera.y + radiusCube;
	A_min.z = centerCamera.z - radiusCube;
	A_max.z = centerCamera.z + radiusCube;

	B_min.x = centerObject.x - radiusCube;
	B_max.x = centerObject.x + radiusCube;
	B_min.y = centerObject.y - radiusCube;
	B_max.y = centerObject.y + radiusCube;
	B_min.z = centerObject.z - radiusCube;
	B_max.z = centerObject.z + radiusCube;

	if(((A_min.x <= B_max.x) && (A_max.x >= B_min.x)) && ((A_min.y <= B_max.y) && (A_max.y >= B_min.y)) && ((A_min.z <= B_max.z) && (A_max.z >= B_min.z))){
		camera.setPosition(cameraBeforeCollision);
	}
	else{
		cameraBeforeCollision = camera.getPosition();
	}
}

void Object::AABBxSphere(Camera& camera){
	// - Gravity action
	if(model[3][1] > model[1][1] + 0.1f) model[3][1] -= Object::forceG;
	if(model[3][1] < model[1][1] + 0.1f) model[3][1] += model[1][1] + 0.1f - model[3][1];

	// - AABB and Sphere collision detection
	vec3<float> centerCamera = camera.getPosition();
	vec3<float> centerObject = vec3<float>(getModel()[3][0], getModel()[3][1], getModel()[3][2]);
	float radiusSphere  = 1.0f; // - Definido temporariamente para 1
	float radiusCube    = 1.0f; // - Definido temporariamente para 1
	vec3<float> A_min;
	vec3<float> A_max;
	A_min.x = centerObject.x - radiusCube;
	A_max.x = centerObject.x + radiusCube;
	A_min.y = centerObject.y - radiusCube;
	A_max.y = centerObject.y + radiusCube;
	A_min.z = centerObject.z - radiusCube;
	A_max.z = centerObject.z + radiusCube;

	vec3<float> closestPoint;
	closestPoint.x = max(A_min.x, min(centerCamera.x, A_max.x));
	closestPoint.y = max(A_min.y, min(centerCamera.y, A_max.y));
	closestPoint.z = max(A_min.z, min(centerCamera.z, A_max.z));

	float distance = length(closestPoint, centerCamera);

	if(distance < radiusSphere){
		vec3<float> dir(closestPoint - centerCamera);
		model = model * translate(dir * 4.0f);
		camera.setPosition(cameraBeforeCollision);
	}
	else{
		cameraBeforeCollision = camera.getPosition();
	}
}

// - Setters

void Object::setName(std::string pname){
	name = pname;
}

void Object::setMaterial(vec3<float> albedo, float roughness, float ao, float metallic){
	setUniform3f("material.albedo", albedo);
	setUniform1f("material.roughness", roughness);
	setUniform1f("material.ao", ao);
	setUniform1f("material.metallic", metallic);
}

void Object::setMaterial(Material& material){
	setUniform3f("material.albedo", &material.albedo);
	setUniform1f("material.roughness", &material.roughness);
	setUniform1f("material.ao", &material.ao);
	setUniform1f("material.metallic", &material.metallic);
}

void Object::setTextures(std::string albedo, std::string normal, std::string roughness, std::string ao, float metallic){
	addTexture(albedo, "material.albedo", 0);
	addTexture(normal, "material.normal", 1);
	addTexture(roughness, "material.roughness", 2);
	addTexture(ao, "material.ao", 3);
	setUniform1f("material.metallic", metallic);
}

// - Getters

uint Object::getVAO() const{
	return VAO;
}

uint Object::getVBO() const{
	return VBO;
}

uint Object::getUBO() const{
	return UBO;
}

uint Object::getShaderType() const{
	return shaderType;
}

std::string Object::getName() const{
	return name;
}

std::vector<float> Object::getVertexBuffer(){
	return vertex;
}

mat4<float>& Object::getModel(){
	return model; 
}

// - New way to load 3d models using Assimp

Mesh::Mesh(std::vector<Vertex> pVertices, std::vector<uint> pIndices, std::vector<Texture> pTextures){
	vertices = pVertices;
	indices  = pIndices;
	textures = pTextures;
	setup();
}

void Mesh::setup(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);					
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // - positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1); // - normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<void*>(sizeof(float) * 3));
	glEnableVertexAttribArray(2); // - texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<void*>(sizeof(float) * 6));
	glBindVertexArray(0);
}

void Mesh::render(uint& program){
	glBindVertexArray(VAO);
	for(uint i = 0; i < SRW::gUBO.size(); i++){
		if(glIsBuffer(SRW::gUBO[i]))
			glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
	}
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &translate(vec3<float>(-10.0f, 0.0f, 0.0f))[0][0]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	
}

Model::Model(std::string path, uint prog){
	loadModel(path);
	program = prog;
}

void Model::render(){
	for(std::vector<Mesh>::iterator it = meshes.begin(); it != meshes.end(); ++it){
		glUseProgram(program);
		(*it).render(program);
		glUseProgram(0);
	}
}

void Model::loadModel(std::string path){
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
	}
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene){
	for(uint i = 0; i < node->mNumMeshes; i++){
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for(uint i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene){
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<Texture> textures;
	for(uint i = 0; i < mesh->mNumVertices; i++){
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
		vertices.push_back(vertex);
	}
	for(uint i = 0; i < mesh->mNumFaces; ++i){
		aiFace face = mesh->mFaces[i];
		for(uint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
		}
	return Mesh(vertices, indices, textures);	
}

// - Útil pois a API Assimp é capaz de dizer que malha possui qual textura de modo que o carregamento de multiplos objetos de um mesmo arquivo
// não requer uma atribuição individual de texturas
std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName){
	std::vector<Texture> textures;
	for(uint i = 0; i < mat->GetTextureCount(type); ++i){
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.id = 0;
		texture.type = typeName;
		texture.path = str;
		textures.push_back(texture);
	}
	return textures;
}