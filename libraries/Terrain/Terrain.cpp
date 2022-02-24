#include "Terrain.hpp"

#if RENDER_DEBUG_MODE
	void Terrain::renderDebugDepth(mat4<float>& viewProj, float zNear, float zFar){
		setProgram(SRW::debugPrograms[0]);
		setUniform1f("zNear", zNear);
		setUniform1f("zFar", zFar);
		setUniformMat4f("debug_depth_model", &model);
		setUniformMat4f("viewProj", &viewProj);
	}
	
	void Terrain::renderDebugNormal(mat4<float>& view, mat4<float>& projection){
		setProgram(SRW::debugPrograms[1]);
		setUniformMat4f("debug_normal_model", &model);	
		setUniformMat4f("view", &view);
		setUniformMat4f("projection", &projection);
	}
#endif

Terrain::Terrain(const char* path, unsigned char bitsPerPixel, unsigned int width, unsigned int height, float hs, float bs){
	heightScale     = hs;
	blockScale      = bs;
	heightmapWidth  = 0;
	heightmapHeight = 0;
	program = SRW::programs[TERRAIN_PROGRAM];
	VAO 	= 0;
	VBO 	= 0;
	IBO 	= 0;
	loadHeightMap(path, bitsPerPixel, width, height);
	setUniformMat4f("model", &model);
}

Terrain::~Terrain(){
	free();
}

void Terrain::free(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	posBuff.clear();
	norBuff.clear();
	uvsBuff.clear();
	indices.clear();
}

void Terrain::getHeightAt(vec3<float>& position){
	float height = -10000000.0f;
	float terrainWidth  = (heightmapWidth - 1) * blockScale;
	float terrainHeight = (heightmapHeight - 1) * blockScale;
	float halfWidth     = terrainWidth * 0.5f;
	float halfHeight    = terrainHeight * 0.5f;

	mat4<float> localMatrix   = inverse(model);
	vec3<float> terrainPos    = position * localMatrix;
	vec3<float> invBlockScale = vec3f(1.0f / blockScale, 0.0f, 1.0f / blockScale);
	vec3<float> offset        = vec3f(halfWidth, 0.0f, halfHeight);
	vec3<float> vertexIndices = (terrainPos + offset) * invBlockScale;

	int u0 = (int)floorf(vertexIndices.x);
	int u1 = u0 + 1;
	int v0 = (int)floorf(vertexIndices.z);
	int v1 = v0 + 1;

	if(u0 >= 0 && u1 < (int)heightmapWidth && v0 >= 0 && v1 < (int)heightmapHeight){
	   	vec3<float> p00 = posBuff[(v0 * heightmapWidth) + u0];    // Top-left vertex
	  	vec3<float> p10 = posBuff[(v0 * heightmapWidth) + u1];    // Top-right vertex
	   	vec3<float> p01 = posBuff[(v1 * heightmapWidth) + u0];    // Bottom-left vertex
	   	vec3<float> p11 = posBuff[(v1 * heightmapWidth) + u1];    // Bottom-right vertex
	    // Which triangle are we over?
	    float percentU = vertexIndices.x - u0;
	    float percentV = vertexIndices.z - v0;
	    vec3<float> dU, dV;
	    if(percentU > percentV){
	    	// Top triangle
	        dU = p10 - p00;
	        dV = p11 - p10;
	    }
	    else{
	    	// Bottom triangle
	    	dU = p11 - p01;
	    	dV = p01 - p00;
	    }
	    vec3<float> heightPos = p00 + (dU * percentU) + (dV * percentV);
	    // Convert back to world-space by multiplying by the terrain's world matrix
	    heightPos = heightPos * model;
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
	posBuff.resize(numVertices);
	norBuff.resize(numVertices);
	uvsBuff.resize(numVertices);
	heightmapWidth          = width;
	heightmapHeight         = height;
	float terrainWidth      = (width  - 1)  * blockScale;
	float terrainHeight     = (height - 1)  * blockScale;
	float halfTerrainWidth  = terrainWidth  * 0.5f;
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
			
			posBuff[index] = vec3<float>(X, Y, Z);
			norBuff[index] = vec3<float>(0.0f, 0.0f, 0.0f);
			uvsBuff[index] = vec2<float>(S, T);
		}
	}
	delete[] heightMap;
	generateIndexBuffer();
	generateNormals();
	generateVertexBuffers();
}

void Terrain::render(){
	glBindVertexArray(VAO);
	for(uint i = 0; i < SRW::gUBO.size(); i++)
		glBindBufferBase(GL_UNIFORM_BUFFER, i, SRW::gUBO[i]);
	initUnifSampler2D();
	initUnif1f();
	initUnif3f();
	initUnifMat4f();
	glUniformMatrix4fv(glGetUniformLocation(SRW::programs[TERRAIN_PROGRAM], "normalSpace"), 1, GL_FALSE, &inverse(model).transpose()[0][0]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Terrain::generateIndexBuffer(){
	if(heightmapWidth < 2 || heightmapHeight < 2){
		return;
	}
	const unsigned int terrainWidth  = heightmapWidth;
	const unsigned int terrainHeight = heightmapHeight;
	const unsigned int numTriangles  = (terrainWidth - 1) * (terrainHeight - 1) * 2;
	indices.resize(numTriangles * 3);
	unsigned int index = 0;
	for(unsigned int j = 0; j < (terrainHeight - 1); ++j){
		for(unsigned int i = 0; i < (terrainWidth - 1); ++i){
			int vertexIndex = (j * terrainWidth) + i;
			//Top triangle
			indices[index++] = vertexIndex;
			indices[index++] = vertexIndex + terrainWidth + 1;
			indices[index++] = vertexIndex + 1;
			//Bottom triangle
			indices[index++] = vertexIndex;
			indices[index++] = vertexIndex + terrainWidth;
			indices[index++] = vertexIndex + terrainWidth + 1;
		}
	}
}

void Terrain::generateNormals(){
	for(uint i = 0; i < indices.size(); i += 3){
		vec3<float> v0 = posBuff[indices[i + 0]];
		vec3<float> v1 = posBuff[indices[i + 1]];
		vec3<float> v2 = posBuff[indices[i + 2]];
		vec3<float> normal = normalize(cross(v1 - v0, v2 - v0));
		norBuff[indices[i + 0]] = norBuff[indices[i + 0]] + normal;
		norBuff[indices[i + 1]] = norBuff[indices[i + 1]] + normal;
		norBuff[indices[i + 2]] = norBuff[indices[i + 2]] + normal;
	}
	for(uint i = 0; i < norBuff.size(); ++i){
		norBuff[i] = normalize(norBuff[i]);
	}
}

void Terrain::generateVertexBuffers(){
	std::vector<float> vertexStream;
	unsigned int average = (posBuff.size() + uvsBuff.size() + norBuff.size()) / 3;
	average = (posBuff.size() == average) && (norBuff.size() == average) && (uvsBuff.size() == average)? average: 0;	
	vec3<float> p1, p2, p3;
	vec2<float> uv1, uv2, uv3;
	vec3<float> edge1, edge2;
	vec2<float> deltaUV1, deltaUV2;
	for(uint i = 0; i < posBuff.size(); i += 3){
		p1  = posBuff[i];
		p2  = posBuff[i + 1];
		p3  = posBuff[i + 2];
		uv1 = uvsBuff[i];
		uv2 = uvsBuff[i + 1]; 
		uv3 = uvsBuff[i + 2]; 
		edge1 = p2 - p1;
		edge2 = p3 - p1;
		deltaUV1 = uv2 - uv1;
		deltaUV2 = uv3 - uv1;
		float f  = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y)? 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y): 0.0f;
		vec3<float> tang;
		tang.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tang.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tang.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		vertexStream.push_back(posBuff[i].x);
		vertexStream.push_back(posBuff[i].y);
		vertexStream.push_back(posBuff[i].z);
		vertexStream.push_back(norBuff[i].x);
		vertexStream.push_back(norBuff[i].y);
		vertexStream.push_back(norBuff[i].z);
		vertexStream.push_back(uvsBuff[i].x);
		vertexStream.push_back(uvsBuff[i].y);
		vertexStream.push_back(tang.x);
		vertexStream.push_back(tang.y);
		vertexStream.push_back(tang.z);

		vertexStream.push_back(posBuff[i + 1].x);
		vertexStream.push_back(posBuff[i + 1].y);
		vertexStream.push_back(posBuff[i + 1].z);
		vertexStream.push_back(norBuff[i + 1].x);
		vertexStream.push_back(norBuff[i + 1].y);
		vertexStream.push_back(norBuff[i + 1].z);
		vertexStream.push_back(uvsBuff[i + 1].x);
		vertexStream.push_back(uvsBuff[i + 1].y);
		vertexStream.push_back(tang.x);
		vertexStream.push_back(tang.y);
		vertexStream.push_back(tang.z);

		vertexStream.push_back(posBuff[i + 2].x);
		vertexStream.push_back(posBuff[i + 2].y);
		vertexStream.push_back(posBuff[i + 2].z);
		vertexStream.push_back(norBuff[i + 2].x);
		vertexStream.push_back(norBuff[i + 2].y);
		vertexStream.push_back(norBuff[i + 2].z);
		vertexStream.push_back(uvsBuff[i + 2].x);
		vertexStream.push_back(uvsBuff[i + 2].y);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(6 * sizeof(float)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(8 * sizeof(float)));
	glBindVertexArray(0);
	// norBuff.clear();
	// uvsBuff.clear();
}

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