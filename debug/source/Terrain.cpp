#include "../headers/world/Terrain.hpp"

Terrain::Terrain(float height_scale, float block_scale, Scene& parm){
		heightScale     = height_scale;
		blockScale      = block_scale;
		heightmapWidth  = 0;
		heightmapHeight = 0;
		program = 0;
		VAO 	= 0;
		VBO 	= 0;
		IBO 	= 0;
		genShader("../shaders/standart/vs_standart.glsl", "../shaders/terrain/fs_terrain.glsl.c");
		setCurrentScene(parm);
}

Terrain::~Terrain(){
	free();
}

void Terrain::free(){
	positionBuffer.clear();
	normalBuffer.clear();
	texcoordBuffer.clear();
	indexBuffer.clear();
}

void Terrain::getHeightAt(vec3f& position){
		float height = -10000000.0f;
		float terrainWidth  = (heightmapWidth - 1) * blockScale;
		float terrainHeight = (heightmapHeight - 1) * blockScale;
		float halfWidth  = terrainWidth * 0.5f;
		float halfHeight = terrainHeight * 0.5f;

		mat4<float> terrainLocalMatrix = (*unifMat4fv[0].value).inverse();
		vec3f terrainPos = position * terrainLocalMatrix;
		vec3f invBlockScale = vec3f(1.0f / blockScale, 0.0f, 1.0f / blockScale);
		vec3f offset = vec3f(halfWidth, 0.0f, halfHeight);
		vec3f vertexIndices = (terrainPos + offset) * invBlockScale;

		int u0 = (int)floorf(vertexIndices.x);
	    int u1 = u0 + 1;
	    int v0 = (int)floorf(vertexIndices.z);
	    int v1 = v0 + 1;

	    if(u0 >= 0 && u1 < (int)heightmapWidth && v0 >= 0 && v1 < (int)heightmapHeight){
	    	vec3f p00 = positionBuffer[(v0 * heightmapWidth) + u0];    // Top-left vertex
	       	vec3f p10 = positionBuffer[(v0 * heightmapWidth) + u1];    // Top-right vertex
	       	vec3f p01 = positionBuffer[(v1 * heightmapWidth) + u0];    // Bottom-left vertex
	       	vec3f p11 = positionBuffer[(v1 * heightmapWidth) + u1];    // Bottom-right vertex
	 
	        // Which triangle are we over?
	        float percentU = vertexIndices.x - u0;
	        float percentV = vertexIndices.z - v0;
	 
	        vec3f dU, dV;
	        if(percentU > percentV)
	        {   // Top triangle
	            dU = p10 - p00;
	            dV = p11 - p10;
	        }
	        else
	        {   // Bottom triangle
	            dU = p11 - p01;
	            dV = p01 - p00;
	        }
	 
	        vec3f heightPos = p00 + (dU * percentU) + (dV * percentV);
	        // Convert back to world-space by multiplying by the terrain's world matrix
	        heightPos = heightPos * translate(vec3f(0.0f, -10.0f, 0.0f));
	        height    = heightPos.y;
    	}
	    position.y = height + 2.7f;
}

void Terrain::loadHeightMap(const char* path, unsigned char bitsPerPixel, unsigned int width, unsigned int height){
		std::ifstream file(path, std::ios_base::binary);
		if(file.fail()){
			std::cerr << "Failed to open file " << path << std::endl;
			return;
		}
		const unsigned int bytesPerPixel = bitsPerPixel / 8;
		const unsigned int expectedFileSize = bytesPerPixel * width * height;
		const unsigned int fileSize = getFileLength(file);
		if(expectedFileSize != fileSize){
			std::cerr << "Expected file size [" << expectedFileSize << " bytes] differs from actual file size [" << fileSize << " bytes]!" << std::endl;
			return;
		}
		unsigned char* heightMap = new unsigned char[fileSize];
		file.read((char*)heightMap, fileSize);
		if(file.fail()){
			std::cerr << "An error occurred while reading height map file:" << path << std::endl;
			file.close();
			delete[] heightMap;
			return;
		}
		unsigned int numVertices = width * height;
		positionBuffer.resize(numVertices);
		texcoordBuffer.resize(numVertices);
		normalBuffer.resize(numVertices);
		heightmapWidth  = width;
		heightmapHeight = height;
		float terrainWidth  = (width - 1) * blockScale;
		float terrainHeight = (height - 1) * blockScale;
		float halfTerrainWidth  = terrainWidth * 0.5f;
		float halfTerrainHeight = terrainHeight * 0.5f;

		for(unsigned j = 0; j < heightmapHeight; ++j){
			for(unsigned i = 0; i < heightmapWidth; ++i){
				unsigned int index = (j * heightmapWidth) + i;
				assert(index * bytesPerPixel < fileSize);
				float heightValue = getHeightValue(&heightMap[index * bytesPerPixel], bytesPerPixel);

				float S = (i / float(heightmapWidth - 1));
				float T = (j / float(heightmapHeight - 1));

				float X = (S * terrainWidth) - halfTerrainWidth;
				float Y = (heightValue * heightScale);
				float Z = (T * terrainHeight) - halfTerrainHeight;
				
				normalBuffer[index]   = vec3f(0.0f, 0.0f, 0.0f);
				positionBuffer[index] = vec3f(X, Y, Z);
				texcoordBuffer[index] = vec2f(S, T);
			}
		}
		delete[] heightMap;
		generateIndexBuffer();
		generateNormals();
		generateVertexBuffers();
}

void Terrain::render(){
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUniforms();
#if DEBUG_VARIABLES_HEIGHTMAP
	glUniform1f(glGetUniformLocation(program, "fWeight"), fWeight);
	glUniform1f(glGetUniformLocation(program, "fTexcoord"), fTexcoord);
#else
	glUniform1f(glGetUniformLocation(program, "fWeight"), 0.65f);
	glUniform1f(glGetUniformLocation(program, "fTexcoord"), 256.0f);
#endif	
	glUniform1f(glGetUniformLocation(program, "fMaxHeight"), heightScale);
	glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, NULL);
	glUseProgram(0);
}

void Terrain::renderMinimap(uint& programID, mat4<float>& view, mat4<float>& model){
	setProgram(programID);
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUnifSampler2D();
#if DEBUG_VARIABLES_HEIGHTMAP
	glUniform1f(glGetUniformLocation(program, "fWeight"), fWeight);
	glUniform1f(glGetUniformLocation(program, "fTexcoord"), fTexcoord);
#else
	glUniform1f(glGetUniformLocation(program, "fWeight"), 0.65f);
	glUniform1f(glGetUniformLocation(program, "fTexcoord"), 1024.0f);
#endif	
	glUniform1f(glGetUniformLocation(program, "fMaxHeight"), heightScale);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_FALSE, &view.matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, &model.matrix[0][0]);
	glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Terrain::renderFromDirectionalLightView(uint& programID, mat4<float>& view, mat4<float>& model){
	glUseProgram(programID);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &model.matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, GL_FALSE, &view.matrix[0][0]);
	glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Terrain::renderFromPointLightView(uint& programID, std::vector<mat4<float>>& transforms, mat4<float>& model, vec3f& light_pos, float zFar){
	glUseProgram(programID);
	glBindVertexArray(VAO);
	glUniform1f(glGetUniformLocation(programID, "far_plane"), zFar);
	glUniform3fv(glGetUniformLocation(programID, "light_pos"), 1, light_pos.address());
	glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &model.matrix[0][0]);
	for(uint i = 0; i < transforms.size(); ++i){
		glUniformMatrix4fv(glGetUniformLocation(programID, std::string("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i].matrix[0][0]);
	}
	glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
}


void Terrain::generateIndexBuffer(){
		if(heightmapWidth < 2 || heightmapHeight < 2){
			return;
		}
		const unsigned int terrainWidth  = heightmapWidth;
		const unsigned int terrainHeight = heightmapHeight;
		const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;
		indexBuffer.resize(numTriangles * 3);
		unsigned int index = 0;
		for(unsigned int j = 0; j < (terrainHeight - 1); ++j){
			for(unsigned int i = 0; i < (terrainWidth - 1); ++i){
				int vertexIndex = (j * terrainWidth) + i;
				//Top triangle
				indexBuffer[index++] = vertexIndex;
				indexBuffer[index++] = vertexIndex + terrainWidth + 1;
				indexBuffer[index++] = vertexIndex + 1;
				//Bottom triangle
				indexBuffer[index++] = vertexIndex;
				indexBuffer[index++] = vertexIndex + terrainWidth;
				indexBuffer[index++] = vertexIndex + terrainWidth + 1;
			}
		}
}

void Terrain::generateNormals(){
		for(unsigned int i = 0; i < indexBuffer.size(); i += 3){
			vec3f v0 = positionBuffer[indexBuffer[i + 0]];
			vec3f v1 = positionBuffer[indexBuffer[i + 1]];
			vec3f v2 = positionBuffer[indexBuffer[i + 2]];
			vec3f normal = normalize(cross(v1 - v0, v2 - v0));
			normalBuffer[indexBuffer[i + 0]] = normalBuffer[indexBuffer[i + 0]] + normal;
			normalBuffer[indexBuffer[i + 1]] = normalBuffer[indexBuffer[i + 1]] + normal;
			normalBuffer[indexBuffer[i + 2]] = normalBuffer[indexBuffer[i + 2]] + normal;
		}
		for(unsigned int i = 0; i < normalBuffer.size(); ++i){
			normalBuffer[i] = normalize(normalBuffer[i]);
		}
}

void Terrain::generateVertexBuffers(){
		std::vector<float> vertexStream;
		unsigned int average = (positionBuffer.size() + texcoordBuffer.size() + normalBuffer.size()) / 3;
		average = (positionBuffer.size() == average) && (normalBuffer.size() == average) && (texcoordBuffer.size() == average)? average: 0;
		
		vec3f p1, p2, p3;
		vec2f uv1, uv2, uv3;
		vec3f edge1, edge2;
		vec2f deltaUV1, deltaUV2;
		for(uint i = 0; i < average; i += 3){
			p1  = positionBuffer[i];
			p2  = positionBuffer[i + 1];
			p3  = positionBuffer[i + 2];
			uv1 = texcoordBuffer[i];
			uv2 = texcoordBuffer[i + 1]; 
			uv3 = texcoordBuffer[i + 2]; 
			edge1 = p2 - p1;
			edge2 = p3 - p1;
			deltaUV1 = uv2 - uv1;
			deltaUV2 = uv3 - uv1;
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			vec3f tang;
			tang.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tang.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tang.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			vertexStream.push_back(positionBuffer[i].x);
			vertexStream.push_back(positionBuffer[i].y);
			vertexStream.push_back(positionBuffer[i].z);
			vertexStream.push_back(normalBuffer[i].x);
			vertexStream.push_back(normalBuffer[i].y);
			vertexStream.push_back(normalBuffer[i].z);
			vertexStream.push_back(texcoordBuffer[i].x);
			vertexStream.push_back(texcoordBuffer[i].y);
			vertexStream.push_back(tang.x);
			vertexStream.push_back(tang.y);
			vertexStream.push_back(tang.z);

			vertexStream.push_back(positionBuffer[i + 1].x);
			vertexStream.push_back(positionBuffer[i + 1].y);
			vertexStream.push_back(positionBuffer[i + 1].z);
			vertexStream.push_back(normalBuffer[i + 1].x);
			vertexStream.push_back(normalBuffer[i + 1].y);
			vertexStream.push_back(normalBuffer[i + 1].z);
			vertexStream.push_back(texcoordBuffer[i + 1].x);
			vertexStream.push_back(texcoordBuffer[i + 1].y);
			vertexStream.push_back(tang.x);
			vertexStream.push_back(tang.y);
			vertexStream.push_back(tang.z);

			vertexStream.push_back(positionBuffer[i + 2].x);
			vertexStream.push_back(positionBuffer[i + 2].y);
			vertexStream.push_back(positionBuffer[i + 2].z);
			vertexStream.push_back(normalBuffer[i + 2].x);
			vertexStream.push_back(normalBuffer[i + 2].y);
			vertexStream.push_back(normalBuffer[i + 2].z);
			vertexStream.push_back(texcoordBuffer[i + 2].x);
			vertexStream.push_back(texcoordBuffer[i + 2].y);
			vertexStream.push_back(tang.x);
			vertexStream.push_back(tang.y);
			vertexStream.push_back(tang.z);
		}
	
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexStream.size(), &vertexStream[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexBuffer.size(), &indexBuffer[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(6 * sizeof(float)));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(8 * sizeof(float)));
		glBindVertexArray(0);
		normalBuffer.clear();
		texcoordBuffer.clear();
}

//Utility

int Terrain::getFileLength(std::ifstream& file){
	int pos = file.tellg();
	file.seekg(0, std::ios::end);
	int length = file.tellg();
	file.seekg(pos);
	return length;		
}

float Terrain::getHeightValue(const unsigned char* data, unsigned char numBytes){
	switch(numBytes){
		case 1:
			return (unsigned char)data[0] / (float)0xff;
		break;
		case 2:
			return (unsigned short)(data[1] << 8 | data[0]) / 0xffff;
		break;
		case 4:
           	return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0] ) / (float)0xffffffff;
		break;
	}
	return 0.0f;
}

#if DEBUG_VARIABLES_HEIGHTMAP
	void Terrain::keyboardEvent(SDL_Event* event){
		if(event->type == SDL_KEYDOWN){
			if(event->key.keysym.scancode == SDL_SCANCODE_KP_1){
				WEIGHT = true;
				TEXCOORD = false;
			}
			if(event->key.keysym.scancode == SDL_SCANCODE_KP_2){
				TEXCOORD = true;
				WEIGHT = false;
			}
			if(WEIGHT || TEXCOORD){
				switch(event->key.keysym.scancode){
					case SDL_SCANCODE_KP_PLUS:
						fWeight = WEIGHT && fWeight < 0.9f? fWeight + 0.01f: fWeight;
						fTexcoord = TEXCOORD && fTexcoord < 2048.0f ? fTexcoord + 10.0f: fTexcoord;
					break;
					case SDL_SCANCODE_KP_MINUS:
						fWeight = WEIGHT && fWeight > 0.1f? fWeight - 0.01f: fWeight;
						fTexcoord = TEXCOORD && fTexcoord > 100.0f ? fTexcoord - 10.0f: fTexcoord;
					break;
				}
			}
		}
	}
	void Terrain::memory_usage(){
	//Não considera variáveis de debuging
		size_t size = 0;
		size  = sizeof(float) * positionBuffer.size();
		size += sizeof(float) * normalBuffer.size();
		size += sizeof(float) * texcoordBuffer.size();
		size += sizeof(GLuint) * indexBuffer.size();
		size += sizeof(GLuint) * 15;
		size += sizeof(float) * 4;
		std::cout << std::setw(10) << "Terrain instance class memory usage: " << size / (1024.0f * 1024.0f) << " megabytes." << std::endl;  
	}
#endif