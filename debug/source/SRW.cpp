#include "../headers/SRW.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stbi/stb_image.h"

using namespace srw;

Camera::Camera(float x, float y, float z, float theta, float near, float far){
	zNear = near;
	zFar  = far;
	yaw 	 		  = -90.0f;
	pitch    		  = 0.0f;
	sensitivity 	  = 0.10f;
	speed   		  = 0.01f;
	fovy    		  = theta;
	position 		  = vec3f(x, y, z);
	target   		  = vec3f(0.0f, 0.0f, -1.0f);
	viewMatrix        = lookAtRH(position, position + target);
	projectionMatrix  = perspective(fovy, (float)screenWidth / (float)screenHeight, zNear, zFar);
	viewAndProjection = viewMatrix * projectionMatrix;
}

void Camera::keyboardEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN && event->key.repeat == 0){
		switch(event->key.keysym.scancode){
			case SDL_SCANCODE_D:
				RIGHT = true;
				break;
			case SDL_SCANCODE_A:
				LEFT = true;
				break;
			case SDL_SCANCODE_W:
				FOWARD = true;
				break;
			case SDL_SCANCODE_S:
				BACKFOWARD = true;
				break;
			default:
				break;
		}
	}
	else if(event->type == SDL_KEYUP && event->key.repeat == 0){
		switch(event->key.keysym.scancode){
			case SDL_SCANCODE_D:
				RIGHT = false;
				break;
			case SDL_SCANCODE_A:
				LEFT = false;
				break;
			case SDL_SCANCODE_W:
				FOWARD = false;
				break;
			case SDL_SCANCODE_S:
				BACKFOWARD = false;
				break;
			default:
				break;
		}
	}
}

void Camera::mouseEvent(SDL_Event* event){
	if(event->type == SDL_MOUSEMOTION){
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		yaw   += x * sensitivity;
		pitch -= y * sensitivity;
		if(pitch > 89.0f){
			pitch      = 89.0f;
			BACKFOWARD = false;
		}
		if(pitch < -89.0f){
			pitch  = -89.0f;
			FOWARD = false;
		}
		updateCamera();
	}
}

void Camera::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			screenWidth  = event->window.data1;
			screenHeight = event->window.data2;
			projectionMatrix  = perspective(fovy, (float)screenWidth / (float)screenHeight, zNear, zFar);
			viewAndProjection = viewMatrix * projectionMatrix;
			glViewport(0, 0, screenWidth, screenHeight); 
		}
	}
}

void Camera::updateMatrices(){
	projectionMatrix  = perspective(fovy, (float)screenWidth / (float)screenHeight, zNear, zFar);
	viewMatrix        = lookAtRH(position, position + target);
	viewAndProjection = viewMatrix * projectionMatrix;
}

void Camera::updateCoordinates(){
	currentFrameTime = SDL_GetTicks();
	deltaTime        = currentFrameTime - lastFrameTime;
	if(RIGHT)
		position = position + (normalize(cross(target, vec3f(0.0f, 1.0f, 0.0f)))) * (speed * deltaTime);
	if(LEFT)
		position = position - (normalize(cross(target, vec3f(0.0f, 1.0f, 0.0f)))) * (speed * deltaTime);
	if(FOWARD)
		position = position + target * (speed * deltaTime);
	if(BACKFOWARD)
		position = position - target * (speed * deltaTime);
	lastFrameTime     = currentFrameTime;
	updateMatrices();
}

void Camera::updateCamera(){
	target.x = cos(radians(yaw)) * cos(radians(pitch));
	target.y = sin(radians(pitch));
	target.z = sin(radians(yaw)) * cos(radians(pitch));
}

void Camera::setFovy(float theta){
	this->fovy = theta;
	updateMatrices();
}

void Camera::setPosition(vec3f coordinate){
	this->position = coordinate;
	updateMatrices();
}

mat4<float>& Camera::getViewAndProjectionMatrix(){
	return viewAndProjection;
}

mat4<float> Camera::getViewMatrix(){
	return viewMatrix;
}

mat4<float>& Camera::getProjectionMatrix(){
	return projectionMatrix;
}

vec3f& Camera::getPosition(){
	return position;
}

vec3f& Camera::getTarget(){
	return target;
}

float Camera::getFovy() const{
	return fovy;
}

float Camera::getYaw() const{
	return yaw;
}

float Camera::getPitch() const{
	return pitch;
}

//SECTION: SRW
//Generate a program variable that can shared between objects
uint SRW::genProgramID(const char* vs_path, const char* fs_path, Scene* parm){
	uint programID = 0;
	bool flag = parm != NULL? writeDinamycVar(fs_path, parm->getLights().size()) : false;
	if(flag || parm == NULL){
		if(createProgram(programID)){
			bool vs_state = createShader(programID, GL_VERTEX_SHADER, readShaderSource(vs_path));
			bool fs_state = createShader(programID, GL_FRAGMENT_SHADER, readShaderSource(fs_path));
			if(vs_state == false || fs_state == false){
				std::cerr << "srw::ERROR::Failed to create shader." << std::endl;
				std::cerr << "srw::WARNING::Exiting..." << std::endl;		
				exit(1);
			}
			else if(!linkProgram(programID)){
				std::cerr << "srw::ERROR::Failed to link to program." << std::endl;
				std::cerr << "srw::WARNING::Exiting..." << std::endl;		
				exit(1);
			}
		}
		return programID;
	}
	else exit(1);
}

uint SRW::genProgramID(const char* vs_path, const char* gs_path, const char* fs_path){
	uint programID = 0;
	if(createProgram(programID)){
		bool vs_state = createShader(programID, GL_VERTEX_SHADER, readShaderSource(vs_path));
		bool gs_state = createShader(programID, GL_GEOMETRY_SHADER, readShaderSource(gs_path));
		bool fs_state = createShader(programID, GL_FRAGMENT_SHADER, readShaderSource(fs_path));
		if(vs_state == false || gs_state == false || fs_state == false){
			std::cerr << "srw::ERROR::Failed to create shader." << std::endl;
			std::cerr << "srw::WARNING::Exiting..." << std::endl;		
			exit(1);
		}
		else if(!linkProgram(programID)){
			std::cerr << "srw::ERROR::Failed to link to program." << std::endl;
			std::cerr << "srw::WARNING::Exiting..." << std::endl;		
			exit(1);
		}
	}
	else
		exit(1);
	return programID;
}

bool SRW::writeDinamycVar(const char* file_path, uint active_lights){
	std::fstream file;
	file.open(file_path, std::ios_base::in | std::ios_base::binary);
	if(!file.is_open()){
		std::cerr << "srw::ERROR::Function <writeInFile> failed to open file!" << std::endl;
		return false;
	}
	else{
		std::filebuf* buffer = file.rdbuf();
		std::string str("#define NR_POINT_LIGHTS");
		std::string line;
		std::vector<std::string> strBuffer;
		char code = buffer->sbumpc();
		while(code != EOF){
			line += code;
			if(code == '\n'){
				uint i = 0;
				while(i < str.size()){
					if(line[i] == str[i]){
						++i;
						continue;
					}
					break;
				}
				if(i == str.size()){
					strBuffer.push_back(str + " " + std::to_string(active_lights) + "\n");
				}
				else{
					strBuffer.push_back(line);
				}
				line.clear();
			} 
			code = buffer->sbumpc();
		}
		file.close();
		file.open(file_path, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		if(!file.is_open()){
			std::cerr << "srw::ERROR::Function <writeInFile> failed to open file!" << std::endl;
			return false;
		}
		else{
			for(uint i = 0; i < strBuffer.size(); ++i){
				file << strBuffer[i];
			}
			file.close();
		}
	}
	return true;
}

//Create a texture in rgb format and a depth buffer
void SRW::genFramebuffer(uint& texID, uint& rboID, uint& fboID, uint width, uint height){
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);

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

//Create a texture which just store depth values

void SRW::genShadowFramebuffer(uint& texID, uint& fboID, uint width, uint height){
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

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

void SRW::genShadowFramebufferFromCubemap(uint& depthCubemap, uint& fboID, uint width, uint height){
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for(uint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

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

void SRW::genGeometryBuffer(uint& gBuffer, uint& gPosition, uint& gNormal, uint& gAlbedo, uint width, uint height){
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

	uint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2}; 
	glDrawBuffers(3, attachments);

	uint rboID = 0;
	glGenRenderbuffers(1, &rboID);
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);
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

//SECTION: Constructors and destructor
SRW::SRW(){
	scene = NULL;
}

SRW::SRW(const char* vs_path, const char* fs_path, Scene& parm){
	if(writeDinamycVar(fs_path, parm.getLights().size())){
		genShader(vs_path, fs_path);
		setCurrentScene(parm);
	}
	else exit(1);
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
	unif3fv.clear();
	unifMat4fv.clear();
	scene = NULL;
}

//SECTION: Shader resources
//Generate a exclusive shader program 
void SRW::genShader(const char* vs_path, const char* fs_path){
	if(createProgram(program)){
		bool vs_state = createShader(program, GL_VERTEX_SHADER, readShaderSource(vs_path));
		bool fs_state = createShader(program, GL_FRAGMENT_SHADER, readShaderSource(fs_path));
		if(vs_state == false || fs_state == false){
			std::cerr << "srw::ERROR::Failed to create shader." << std::endl;
			std::cerr << "srw::WARNING::Exiting..." << std::endl;		
			exit(1);
		}
		else if(!linkProgram(program)){
			std::cerr << "srw::ERROR::Failed to link to program." << std::endl;
			std::cerr << "srw::WARNING::Exiting..." << std::endl;		
			exit(1);
		}
	}
}

void SRW::initUnifSampler2D(){
	if(!sampler2D.empty()){
		for(uint i = 0; i < sampler2D.size(); ++i){
			glActiveTexture(GL_TEXTURE0 + sampler2D[i].value);
			glBindTexture(GL_TEXTURE_2D, sampler2D[i].texID);
			glUniform1i(glGetUniformLocation(program, sampler2D[i].uniformName.c_str()), sampler2D[i].value);
		}
	}
}

void SRW::initUnifSamplerCubemap(){
	if(!samplerCubemap.empty()){
		for(uint i = 0; i < samplerCubemap.size(); ++i){
			glActiveTexture(GL_TEXTURE0 + samplerCubemap[i].value);
			glBindTexture(GL_TEXTURE_CUBE_MAP, samplerCubemap[i].texID);
			glUniform1i(glGetUniformLocation(program, samplerCubemap[i].uniformName.c_str()), samplerCubemap[i].value);
		}
	}
}

void SRW::initUnif1b(){
	if(!unif1b.empty()){
		for(uint i = 0; i < unif1b.size(); ++i){
			glUniform1i(glGetUniformLocation(program, unif1b[i].uniformName.c_str()), unif1b[i].value);
		}
	}
}

void SRW::initUnif1f(){
	if(!unif1f.empty()){
		for(uint i = 0; i < unif1f.size(); ++i){
			if(unif1f[i].ptrValue == NULL)
				glUniform1f(glGetUniformLocation(program, unif1f[i].uniformName.c_str()), unif1f[i].value);
			else
				glUniform1f(glGetUniformLocation(program, unif1f[i].uniformName.c_str()), *unif1f[i].ptrValue);				
		}
	}
}

void SRW::initUnif3f(){
	if(!unif3f.empty()){
		for(uint i = 0; i < unif3f.size(); ++i){
			glUniform3fv(glGetUniformLocation(program, unif3f[i].uniformName.c_str()), 1, unif3f[i].value.address());
		}
	}
}

void SRW::initUnif3fv(){
	if(!unif3fv.empty()){
		for(uint i = 0; i < unif3fv.size(); ++i){
			glUniform3fv(glGetUniformLocation(program, unif3fv[i].uniformName.c_str()), 1, unif3fv[i].value->address());
		}
	}
}

void SRW::initUnifMat4fv(){
	if(!unifMat4fv.empty()){
		for(uint i = 0; i < unifMat4fv.size(); ++i){
			glUniformMatrix4fv(glGetUniformLocation(program, unifMat4fv[i].uniformName.c_str()), 1, GL_FALSE, &unifMat4fv[i].value->matrix[0][0]);
		}
	}
}

void SRW::initUnifPointLights(){
	if(scene != NULL){
		for(uint i = 0; i < scene->getLights().size(); ++i){
			std::string index = std::to_string(i);
			glUniform3fv(glGetUniformLocation(program, ("pointLight[" + index + "].position").c_str()), 1, scene->getLights()[i]->getPosition().address());
			glUniform3fv(glGetUniformLocation(program, ("pointLight[" + index + "].ambient").c_str()), 1, scene->getLights()[i]->getAmbient().address());
			glUniform3fv(glGetUniformLocation(program, ("pointLight[" + index + "].diffuse").c_str()), 1, scene->getLights()[i]->getDiffuse().address());
			glUniform3fv(glGetUniformLocation(program, ("pointLight[" + index + "].specular").c_str()), 1, scene->getLights()[i]->getSpecular().address());
			glUniform1f(glGetUniformLocation(program, ("pointLight[" + index + "].constant").c_str()), scene->getLights()[i]->getConstantValue());
			glUniform1f(glGetUniformLocation(program, ("pointLight[" + index + "].linear").c_str()), scene->getLights()[i]->getLinearValue());
			glUniform1f(glGetUniformLocation(program, ("pointLight[" + index + "].quadratic").c_str()), scene->getLights()[i]->getQuadraticValue());
		}
	}
}

void SRW::initUnifGlobalLight(){
	if(scene != NULL){
		glUniform3fv(glGetUniformLocation(program, "directionalLight.direction"), 1, scene->getWorldLight().getDirection().address());
		glUniform3fv(glGetUniformLocation(program, "directionalLight.ambient"), 1, scene->getWorldLight().getAmbient().address());
		glUniform3fv(glGetUniformLocation(program, "directionalLight.diffuse"), 1, scene->getWorldLight().getDiffuse().address());
		glUniform3fv(glGetUniformLocation(program, "directionalLight.specular"), 1, scene->getWorldLight().getSpecular().address());
	}
}

void SRW::initUniforms(){
	initUnifSampler2D();
	initUnifSamplerCubemap();
	initUnif1b();
	initUnif1f();
	initUnif3f();
	initUnif3fv();
	initUnifMat4fv();
	initUnifPointLights();
	initUnifGlobalLight();
}

//SECTION: Setters to uniform instaces
void SRW::setUniform1b(std::string name, bool value){
	this->unif1b.push_back(uniform1b(name, value));
}

void SRW::setUniform1f(std::string name, float value){
	this->unif1f.push_back(uniform1f(name, value));
}

void SRW::setUniform1f(std::string name, float* value){
	this->unif1f.push_back(uniform1f(name, value));
}

void SRW::setUniform3f(std::string name, vec3f value){
	this->unif3f.push_back(uniform3f(name, value));
}

void SRW::setUniform3fv(std::string name, vec3f* value){
	this->unif3fv.push_back(uniform3fv(name, value));
}

void SRW::setUniformMat4fv(std::string name, mat4<float>* value){
	this->unifMat4fv.push_back(uniformMat4fv(name, value));
}

//Reset uniforms values
void SRW::setUniform1b(uint index, bool value){
	unif1b[index].value = value;
}

void SRW::setUniform1f(uint index, float value){
	unif1f[index].value = value;
}

void SRW::setUniform3f(uint index, vec3f value){
	unif3f[index].value = value;
}

void SRW::addTexture(const char* texPath, const char* uniformName, uint texUnit, bool SRGB, bool SRGBA, uint* parmWidth, uint* parmHeight){
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
		if(SRGB)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);			
		else if(SRGBA)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);			
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED + channels, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);
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

void SRW::setCurrentScene(Scene& parm){
	this->scene = &parm;
}

void SRW::setProgram(uint& parm){
	this->program = parm;
}

//SECTION: Getters
uint SRW::getProgram() const{
	return program;
}

//Search by name at uniform vectors
uint SRW::searchUniform1b(std::string name){
	uint i = 0;
	while((name.compare(unif1b[i].uniformName) != 0) && i < unif1b.size()){
		i++;
	}
	if(name.compare(unif1b[i].uniformName) == 0){
		return i;
	}
	std::cout << "Uniform name is not found! Returned first element." << std::endl;
	return 0;
}

uint SRW::searchUniform1f(std::string name){
	uint i = 0;
	while((name.compare(unif1f[i].uniformName) != 0) && i < unif1f.size()){
		i++;
	}
	if(name.compare(unif1f[i].uniformName) == 0){
		return i;
	}
	std::cout << "Uniform name is not found! Returned first element." << std::endl;
	return 0;
}

uint SRW::searchUniform3f(std::string name){
	uint i = 0;
	while((name.compare(unif3f[i].uniformName) != 0) && i < unif3f.size()){
		i++;
	}
	if(name.compare(unif3f[i].uniformName) == 0){
		return i;
	}
	std::cout << "Uniform name is not found! Returned first element." << std::endl;
	return 0;
}

uint SRW::searchUniform3fv(std::string name){
	uint i = 0;
	while((name.compare(unif3fv[i].uniformName) != 0) && i < unif3fv.size()){
		i++;
	}
	if(name.compare(unif3fv[i].uniformName) == 0){
		return i;
	}
	std::cout << "Uniform name is not found! Returned first element." << std::endl;
	return 0;
}

uint SRW::searchUniformMat4fv(std::string name){
	uint i = 0;
	while((name.compare(unifMat4fv[i].uniformName) != 0) && i < unifMat4fv.size()){
		i++;
	}
	if(name.compare(unifMat4fv[i].uniformName) == 0){
		return i;
	}
	std::cout << "Uniform name is not found! Returned first element." << std::endl;
	return 0;
}

//SECTION: Instance information
void SRW::info(){
	std::cout << std::setw(12) <<"Program ID: " << program << "\n" << std::endl;
	if(scene != NULL){
		for(uint i = 0; i < scene->getLights().size(); ++i){
			scene->getLights()[i]->show();
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << std::setw(12) << "\nMatrices:\n" << std::endl;
	for(uint i = 0; i < unifMat4fv.size() && !unifMat4fv.empty(); ++i){
		unifMat4fv[i].value->show(unifMat4fv[i].uniformName);
	}
	std::cout << std::setw(12) << "\nSampler2D:\n" << std::endl;
	for(uint i = 0; i < sampler2D.size() && !sampler2D.empty(); ++i){
		std::cout << "[" << sampler2D[i].texID << "]" << std::endl;
	}
	std::cout << std::setw(12) << "\nSamplerCubemap:\n" << std::endl;
	for(uint i = 0; i < samplerCubemap.size() && !samplerCubemap.empty(); ++i){
		std::cout << std::setw(20) << samplerCubemap[i].uniformName << " [" << samplerCubemap[i].value << "]" << std::endl;
	}
	std::cout << std::setw(12) << "\nUniform1f:\n" << std::endl;
	for(uint i = 0; i < unif1f.size() && !unif1f.empty(); ++i){
		std::cout << std::setw(20) << unif1f[i].uniformName << " [" << unif1f[i].value << "]" << std::endl;
	}
	std::cout << std::setw(12) << "\nUniform3f:\n" << std::endl;
	for(uint i = 0; i < unif3f.size() && !unif3f.empty(); ++i){
		unif3f[i].value.show(unif3f[i].uniformName);	
	}
	std::cout << std::setw(12) << "\nUniform3fv:\n" << std::endl;
	for(uint i = 0; i < unif3fv.size() && !unif3fv.empty(); ++i){
		unif3fv[i].value->show(unif3fv[i].uniformName);
	}
}

//SECTION: General objects
#if DEBUG
//Object: Static members and functions used to move objects in the scene
std::vector<Object*> Object::collection;
std::vector<int> Object::offset;
uint Object::programPicking = 0;
uint Object::UBO            = 0;
uint Object::activeIndex    = 0;
uchar Object::data 			= '\0';
vec2f Object::currentOffset = vec2f(0.0f);
vec2f Object::deltaOffset   = vec2f(0.0f);   
vec2f Object::latestOffset  = vec2f(0.0f);
bool Object::selected 		= false;
bool Object::XAXIS 			= false;
bool Object::YAXIS 			= false;
bool Object::ZAXIS 			= false;
int Object::mouseX 			= 0;
int Object::mouseY 			= 0;
int Object::blockSize       = -1;
uchar* Object::blockBuffer  = NULL;

Object::Object(){
	if(parser("models/cube.obj")){
		genBuffers();
		uint size = Object::collection.size();
		std::vector<std::string> shader = {"#version 460\n",
									   "layout(location = 0) in vec4 position;\n",
									   "out vec4 out_color;\n",
			  						   "uniform mat4 viewMatrix;\n",
									   "layout(binding = 0) uniform object{\n",
									   "vec4 color[" + std::to_string(size) + "];\n",
									   "mat4 modelMatrix[" + std::to_string(size) + "];\n",
									   "}obj;\n",
									   "void main(){\n",
									   "out_color   = obj.color[gl_InstanceID];\n",
									   "gl_Position = viewMatrix * obj.modelMatrix[gl_InstanceID] * position;\n",
									   "}"
					    			};
		std::string vs;
		std::string fs;
		std::vector<std::string>::iterator it;
		for(it = shader.begin(); it < shader.end(); ++it){
			vs += *it;
		}
		shader.clear();
		shader = {"#version 460\n",
				  "in vec4 out_color;\n",
				  "out vec4 fragment;\n",
				  "void main(){\n",
					"fragment = out_color;\n",
				  "}"
				};
		for(it = shader.begin(); it < shader.end(); ++it){
			fs += *it;
		}
		createProgram(Object::programPicking);
		createShader(Object::programPicking, GL_VERTEX_SHADER, vs);
		createShader(Object::programPicking, GL_FRAGMENT_SHADER, fs);
		linkProgram(Object::programPicking);
		Object::createUniformBufferData();
		setProgram(Object::programPicking);
	}
}

//Create a uniform buffer to instanced rendering used at the objects in color picking mode
void Object::createUniformBufferData(){
	uint blockIndex = glGetUniformBlockIndex(Object::programPicking, "object");
	if(blockIndex == GL_INVALID_INDEX)
		std::cerr << "ERROR::Uniformblock name is not have a valid index!" << std::endl;
	else{
		Object::blockSize = -1;
		glGetActiveUniformBlockiv(Object::programPicking, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &Object::blockSize);
		if(Object::blockSize == -1)
			std::cerr << "ERROR::An error occurred getting the minimum size for the buffer!" << std::endl;
		else{
			Object::blockBuffer = new uchar[Object::blockSize];
			uint countMembers = 2;
			const char* names[] = {"object.color", "object.modelMatrix"};
			uint indices[countMembers];
			glGetUniformIndices(Object::programPicking, countMembers, names, indices);
			bool success = true;
			for(uint index = 0; index < countMembers; ++index){
				switch(indices[index]){
					case GL_INVALID_INDEX:
						std::cerr << names[index] << " is not a program variable shader or is not active!" << std::endl;
						success = false;
						break;
					default:
						break;
				}
			}
			if(success){
				Object::offset.resize(countMembers);
				glGetActiveUniformsiv(Object::programPicking, countMembers, indices, GL_UNIFORM_OFFSET, offset.data());
				for(uint i = 0; i < Object::collection.size(); ++i){
					vec4f color((float)i / 255.0f, 0.0f, 0.0f, 1.0f);
					Object::collection[i]->setUniform3f("color", vec3f(color.r, color.g, color.b));
					mat4<float> temp = Object::collection[i]->getUniformMat4fv(Object::collection[i]->searchUniformMat4fv("modelMatrix"));
					std::memcpy(blockBuffer + offset[0] + (sizeof(vec4f) * i), &color, sizeof(vec4f));
					std::memcpy(blockBuffer + offset[1] + (sizeof(mat4<float>) * i), temp.matrix, sizeof(mat4<float>));
				}
				glGenBuffers(1, &Object::UBO);
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, Object::UBO);
				glBufferData(GL_UNIFORM_BUFFER, Object::blockSize, Object::blockBuffer, GL_DYNAMIC_DRAW);
			}
		}
	}	
}

//Update uniform buffer if picking event is detected
void Object::updateUniformBufferData(){
		for(uint i = 0; i < Object::collection.size(); ++i){
			vec4f color((float)i / 255.0f, 0.0f, 0.0f, 1.0f);
			Object::collection[i]->setUniform3f(Object::collection[i]->searchUniform3f("color"), vec3f(color.r, color.g, color.b));
			mat4<float> temp = Object::collection[i]->getUniformMat4fv(Object::collection[i]->searchUniformMat4fv("modelMatrix"));
			std::memcpy(blockBuffer + offset[0] + (sizeof(vec4f) * i), &color, sizeof(vec4f));
			std::memcpy(blockBuffer + offset[1] + (sizeof(mat4<float>) * i), temp.matrix, sizeof(mat4<float>));
		}
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, Object::UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, Object::blockSize, Object::blockBuffer);
}

void Object::updateModelViewMatrix(){
	currentOffset.x = Object::mouseX;
	deltaOffset.x   = currentOffset.x - latestOffset.x;
	deltaOffset.x   = deltaOffset.x > 100.0f? 0.0f: deltaOffset.x;
	latestOffset.x  = currentOffset.x;
	currentOffset.y = Object::mouseY;
	deltaOffset.y   = currentOffset.y - latestOffset.y;
	deltaOffset.y   = deltaOffset.y > 100.0f? 0.0f: deltaOffset.y;
	latestOffset.y  = currentOffset.y;
	if(Object::XAXIS){
		// std::cout << deltaOffset.x << std::endl;
		*unifMat4fv[0].value = (*unifMat4fv[0].value) * translate(vec3f(deltaOffset.x * 0.01f, 0.0f, 0.0f));
	}
	if(Object::YAXIS){
		// std::cout << deltaOffset.y << std::endl;
		*unifMat4fv[0].value = (*unifMat4fv[0].value) * translate(vec3f(0.0f, deltaOffset.y * -0.01f, 0.0f));
	}
	if(Object::ZAXIS){
		// std::cout << deltaOffset.x << std::endl;
		*unifMat4fv[0].value = (*unifMat4fv[0].value) * translate(vec3f(0.0f, 0.0f, deltaOffset.x * 0.01f));
	}
	updateUniformBufferData();
}

void Object::picking(SDL_Event* event){
	SDL_GetMouseState(&Object::mouseX, &Object::mouseY);
	if(event->button.type == SDL_MOUSEBUTTONDOWN){
		for(uint i = 0; i < collection.size(); ++i){
			if(((float)Object::data / 255.0f) == collection[i]->getUniform3f(collection[i]->searchUniform3f("color")).r){
				std::cout << "Object Color ID: " << (float)Object::data << std::endl;
				activeIndex = i;
				data = '\0';
				Object::selected = true;
				break;
			}
		}
	}
	else if(event->button.type == SDL_MOUSEBUTTONUP){
		Object::selected = false;
	}
	if(Object::selected){
		if(event->type == SDL_KEYDOWN && event->key.repeat == 0){
			switch(event->key.keysym.scancode){
				case SDL_SCANCODE_X:
					Object::XAXIS = true;
					break;
				case SDL_SCANCODE_Y:
					Object::YAXIS = true;
					break;
				case SDL_SCANCODE_Z:
					Object::ZAXIS = true;
					break;
				default:
					break;
			}
		}
		if(event->type == SDL_MOUSEMOTION){
			SDL_SetWindowGrab(window, SDL_TRUE);
			if((XAXIS || YAXIS) || ZAXIS){
				Object::collection[activeIndex]->updateModelViewMatrix();
			}
		}
	}
	else{
		Object::XAXIS = false;
		Object::YAXIS = false;
		Object::ZAXIS = false;
		SDL_SetWindowGrab(window, SDL_FALSE);
	}
}

void Object::renderColorPicking(){
	glUseProgram(program);
	glBindVertexArray(VAO);	
	initUniforms();	
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertex.size(), Object::collection.size());
	glBindVertexArray(0);
	glUseProgram(0);
	glReadPixels(Object::mouseX, screenHeight - Object::mouseY, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &Object::data);
}
#endif

Object::Object(const char* model_path, const char* vs_path, const char* fs_path, Scene& parm){
	#if DEBUG
	Object::collection.push_back(this);
	#endif
	if(parser(model_path)){
		genShader(vs_path, fs_path);
		setCurrentScene(parm);
		genBuffers();
	}
}

Object::Object(const char* model_path, const char* vs_path, const char* fs_path){
	#if DEBUG
	Object::collection.push_back(this);
	#endif
	if(parser(model_path)){
		genShader(vs_path, fs_path);
		genBuffers();
	}
}

Object::Object(const char* model_path, uint& programID, Scene& parm){
	#if DEBUG
	Object::collection.push_back(this);
	#endif
	if(parser(model_path)){
		setProgram(programID);
		setCurrentScene(parm);
		genBuffers();
	}
}

Object::Object(const char* model_path, uint& programID){
	#if DEBUG
	Object::collection.push_back(this);
	#endif
	if(parser(model_path)){
		setProgram(programID);
		genBuffers();
	}
}

Object::~Object(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
#if DEBUG
	glDeleteBuffers(1, &UBO);
#endif
	vertex.clear();
}

void Object::setDiffuseMap(std::string path){
	addTexture(("../assets/textures/" + path).c_str(), "material.diffuse", 0, true, false);
}

void Object::setSpecularMap(std::string path){
	addTexture(("../assets/textures/" + path).c_str(), "material.specular", 1, true, false);
}

void Object::setDisplacementMap(std::string path){
	addTexture(("../assets/textures/" + path).c_str(), "material.depth", 2, true, false);
}

void Object::setNormalMap(std::string path){
	addTexture(("../assets/textures/" + path).c_str(), "material.normal", 3, true, false);
}

void Object::boundingBox(){
	vec3f t, s;
	t.x = unifMat4fv[searchUniformMat4fv("modelMatrix")].value->matrix[3][0];
	t.y = unifMat4fv[searchUniformMat4fv("modelMatrix")].value->matrix[3][1];
	t.z = unifMat4fv[searchUniformMat4fv("modelMatrix")].value->matrix[3][2];
	mat4<float> transform;
	transform.matrix[3][0] = t.x;
	transform.matrix[3][1] = t.y;
	transform.matrix[3][2] = t.z;
	transform.matrix[0][0] = scale_box.x;
	transform.matrix[1][1] = scale_box.y;
	transform.matrix[2][2] = scale_box.z;
	pFowardTopRight = vec3f(1.0f, 1.0f, 1.0f)    * transform;
	pFowardTopLeft  = vec3f(-1.0f, 1.0f, 1.0f)   * transform;
	pFowardBotRight = vec3f(1.0f, -1.0f, 1.0f)   * transform;
	pFowardBotLeft  = vec3f(-1.0f, -1.0f, 1.0f)  * transform;
	pBackTopRight   = vec3f(1.0f, 1.0f, -1.0f)   * transform;
	pBackTopLeft    = vec3f(-1.0f, 1.0f, -1.0f)  * transform;
	pBackBotRight   = vec3f(1.0f, -1.0f, -1.0f)  * transform;
	pBackBotLeft    = vec3f(-1.0f, -1.0f, -1.0f) * transform;
}

void Object::collisionDetection(Object* obj){
	boundingBox();
	bool collision = true;
	if(pFowardTopLeft.x >= obj->pFowardTopRight.x)
		collision = false;
	if(pFowardTopRight.x <= obj->pFowardTopLeft.x)
		collision = false;
	
	if(pFowardBotRight.y >= obj->pFowardTopRight.y)
		collision = false;
	if(pFowardTopRight.y <= obj->pFowardBotRight.y)
		collision = false;

	if(pBackTopRight.z >= obj->pFowardTopRight.z)
		collision = false;
	if(pFowardTopRight.z <= obj->pBackTopRight.z)
		collision = false;

	if(collision){
		std::cout << "collision on\n";
	}

}

void Object::genBuffers(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// glGenBuffers(1, &IBO);
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
	glUseProgram(program);
	glBindVertexArray(VAO);	
	initUniforms();
	initUnifSampler2D();
	initUnifSamplerCubemap();
	initUnif1f();
	initUnif3fv();
	initUnifMat4fv();
	initUnifPointLights();
	initUnifGlobalLight();
	glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 11);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Object::renderMinimap(uint& programID, mat4<float>& view, mat4<float>& model){
	setProgram(programID);
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUnifSampler2D();
	glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, &view.matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &model.matrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 11);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Object::renderFromDirectionalLightView(uint& programID, mat4<float>& view, mat4<float>& model){
	glUseProgram(programID);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &model.matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, GL_FALSE, &view.matrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 11);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Object::renderFromPointLightView(uint& programID, std::vector<mat4<float>>& transforms, mat4<float>& model, vec3f& light_pos, float zFar){
	glUseProgram(programID);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &model.matrix[0][0]);
	glUniform1f(glGetUniformLocation(programID, "zFar"), zFar);
	glUniform3fv(glGetUniformLocation(programID, "light_pos"), 1, light_pos.address());
	for(uint i = 0; i < transforms.size(); ++i)
		glUniformMatrix4fv(glGetUniformLocation(programID, std::string("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i].matrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 11);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Object::search(std::vector<float>* container, std::ifstream* file, char code){
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

bool Object::parser(const char* model_path){
	std::ifstream file(model_path, std::ios_base::in);
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
				//Obtém a escala do objeto para teste de colisão
				case 's':
					file.get(code);
					if(code == 'x'){
						file.get(code);
						while(code != '\n'){
							file.get(code);
							strData += code;
							scale_box.x = std::stof(strData);
						}
					}
					else if(code == 'y'){
						file.get(code);
						while(code != '\n'){
							file.get(code);
							strData += code;
							scale_box.y = std::stof(strData);
						}
					}
					else if(code == 'z'){
						file.get(code);
						while(code != '\n'){
							file.get(code);
							strData += code;
							scale_box.z = std::stof(strData);
						}
					}
					strData.clear();
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
		vec3f p1, p2, p3;
		vec2f uv1, uv2, uv3;
		vec3f edge1, edge2;
		vec2f deltaUV1, deltaUV2;
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
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			vec3f tang;
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

std::vector<float> Object::getVertexBufferArray(){
	return vertex;
}

std::string Object::getName(){
	return name;
}

uint Object::getVAO() const{
	return VAO;
}

uint Object::getVBO() const{
	return VBO;
}

//SECTION: class Skybox
mat4<float> Skybox::viewAndProjection = translate(vec3f(0.0f));
Skybox::Skybox(std::string folder, float min){
	#if DEBUG
	Object::collection.pop_back();
	#endif
	opacity 	   = 1.0f;
	cycle   	   = false;
	duration 	   = min;
	unityLightness = 0.01f;
	unityOpacity   = unityLightness * 2.0f;
    cube.setUniformMat4fv("view", &Skybox::viewAndProjection);
	//Coordenadas UVs e Normais não são usadas
	std::vector<std::string> path = {folder + "/right.jpg",
									 folder + "/left.jpg",
									 folder + "/top.jpg",
									 folder + "/bottom.jpg",
									 folder + "/front.jpg",
									 folder + "/back.jpg"
									};
	uint id      = 0;
	int width    = 0;
	int height   = 0;
	int channels = 0;
	uchar* data  = NULL;
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for(unsigned i = 0; i < path.size(); ++i){
		data = stbi_load(path[i].c_str(), &width, &height, &channels, 0);
		if(data){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else{
			std::cout << "srw::ERROR::Failed to load texture: " << path[i] << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
	cube.addTexture(id, "skybox", 0, 1);
}

Skybox::~Skybox(){
	//Nothing to do
}

void Skybox::timeline(float ms){
	time += ms;
	if(time >= duration * 60.0f){
		if(lightness.r <= 0.0f && !cycle)
			cycle = true;
		else if(lightness.r >= 1.0f && cycle)
			cycle = false;
		if(cycle){
			opacity -= unityOpacity;
			lightness.r += unityLightness;
			lightness.g += unityLightness;
			lightness.b += unityLightness;
		}
		else{
			opacity += unityOpacity;
			lightness.r -= unityLightness;
			lightness.g -= unityLightness;
			lightness.b -= unityLightness;
		}
		time = 0;
	}
}

void Skybox::render(){
    glFrontFace(GL_CW); 
	glDepthMask(GL_FALSE);
	glUseProgram(cube.getProgram());
	glBindVertexArray(cube.getVAO());
	cube.initUnifSamplerCubemap();
	cube.initUnifMat4fv();
	glUniform1f(glGetUniformLocation(cube.getProgram(), "opacity"), opacity);
	glDrawArrays(GL_TRIANGLES, 0, cube.getVertexBufferArray().size());
	glBindVertexArray(0);
	glUseProgram(0);
	glDepthMask(GL_TRUE);
    glFrontFace(GL_CCW); 
}

void Skybox::setOpacity(float alpha){
	this->opacity = alpha;
}

float Skybox::getOpacity() const{
	return opacity;
}

void Skybox::setLightness(float r, float g, float b){
		this->lightness = vec3f(r, g, b);
}

vec3f Skybox::getLightness(){
	return lightness;
}

bool Skybox::getCycleState() const{
	return cycle;
}

void Skybox::setCycleState(bool state){
	cycle = state;
}

//SECTION: class Text
Text::Text(const char* path, uint size){
	genText(path, size);
	clip      = false;
	borderMin = 0.0f;
	borderMax = 0.0f;
	color = vec3f(1.0f);
}

void Text::genText(const char* path, uint size){
	FT_Library ft;
	if(FT_Init_FreeType(&ft)){
		std::cerr << "srw::Error: Couldn't init freeType library!" << std::endl;
	}
	FT_Face face;
	if(FT_New_Face(ft, path, 0, &face)){
		std::cerr << "srw::Error: Failed to find font!" << std::endl;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FT_Set_Pixel_Sizes(face, 0, size);
	for(uchar c = 0; c < 128; ++c){
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
			std::cerr << "srw::Error: Failed to load glyph!" << std::endl;
			continue;
		}
		uint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		Glyph glyph = {	texture, 
						face->glyph->bitmap.width, face->glyph->bitmap.rows,
						face->glyph->bitmap_left, face->glyph->bitmap_top, 
						face->glyph->advance.x
					  };
		glyphMap.insert(std::pair<char, Glyph>(c, glyph));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	genShader("../shaders/text/vs_text.glsl", "../shaders/text/fs_text.glsl");
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Text::render(std::string text, float x, float y, float scale){
	glUseProgram(program);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "fontTex"), 0);
	glUniform3fv(glGetUniformLocation(program, "colorText"), 1, color.address());

	//Clip object if your edges are greater that the object container
	glUniform1i(glGetUniformLocation(program, "clipping"), clip);
	glUniform1f(glGetUniformLocation(program, "borderMin"), borderMin);
	glUniform1f(glGetUniformLocation(program, "borderMax"), borderMax);

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &ortho(screenWidth, 0, screenHeight, 0).matrix[0][0]);
	std::string::const_iterator c;
	for(c = text.begin(); c != text.end(); c++){
		Glyph glyph = glyphMap[*c];
		float xpos = x + glyph.bearingX * scale;
		float ypos = y - (glyph.height - glyph.bearingY) * scale;
		float w = glyph.width * scale;
		float h = glyph.height * scale;
		float vertices[6][4]{
			{ xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }  
		};
		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (glyph.advance >> 6) * scale;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Text::setColor(float r, float g, float b){
	this->color = vec3f(r, g, b);
}

void Text::setPosition(float x, float y){
	this->position = vec2f(x, y);
}

void Text::clipping(bool flag){
	this->clip = flag;
}

void Text::setBorderMin(float value){
	this->borderMin = value;
}

void Text::setBorderMax(float value) {
	this->borderMax = value;
}

vec2f Text::getSize(std::string text){
	vec2f size;
	std::string::const_iterator c;
	for(c = text.begin(); c != text.end(); c++){
		size.w += glyphMap[*c].width;
		size.h  = glyphMap[*c].height;
	}
	return size;
}

vec2f Text::getPosition(){
	return position;
}

void Text::free(){
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

//SECTION: UI class base
mat4<float> UI::uiMatrix = ortho(screenWidth, 0.0f, screenHeight, 0.0f);
bool UI::lockedUI = false;

UI::UI(){
	//As variáves on, out e over são usadas para realçar a cor de objetos UI
	//de acordo com a ação aplicada - na pratica apenas botões as utilizam.
	on   = 0.6f;
	out  = 0.8f;
	over = 1.0f;

	//Objetos possuem rótulos que os identificam 
	uiName     = "UNKNOWNNAME";
	fontFamily = "../assets/fonts/bahnschrift.ttf";
	fontSize   = 12.0f;
	
	//Essas váriaveis indicam a relação semântica entre objetos UI
	//Coordenadas, dimensões e comportamentos de objetos filhos são relativos ao objeto pai
	parent         = NULL; 
	//Define o objeto a ser invocado a partir do objeto atual
	objectSummon   = NULL;
	objectSummoner = NULL;
	
	//Define o estado atual do objeto, se vísivel ou oculto
	hided      = false;
	enableUI   = true;

	//Identificadores para objetos OpenGL
	VAO        = 0;
	VBO        = 0;
	//Define a matriz que transforma unidades de pixel em NDC
	UI::uiMatrix = ortho(screenWidth, 0.0f, screenHeight, 0.0f);
	setUniformMat4fv("uiMatrix", &UI::uiMatrix);
}

UI::~UI(){
	child.clear();
	parent            = NULL;
	objectSummon      = NULL;
	objectSummoner    = NULL;
	hided		      = true;
	inside		      = false;
	resetActiveRegion = false;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void UI::genBuffer(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	float vertices[] = { position.x, position.y + resolution.h, 0.0f, 0.0f,
				         position.x, position.y, 0.0f, 1.0f,
					  	 position.x + resolution.w, position.y, 1.0f, 1.0f,
					  	 position.x + resolution.w, position.y, 1.0f, 1.0f,
					  	 position.x + resolution.w, position.y + resolution.h, 1.0f, 0.0f,
					  	 position.x, position.y + resolution.h, 0.0f, 0.0f 
						};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glBindVertexArray(0);
}

void UI::updateBuffer(){
	float vertices[] = { position.x, position.y + resolution.h, 0.0f, 0.0f,
				         position.x, position.y, 0.0f, 1.0f,
					  	 position.x + resolution.w, position.y, 1.0f, 1.0f,
					  	 position.x + resolution.w, position.y, 1.0f, 1.0f,
					  	 position.x + resolution.w, position.y + resolution.h, 1.0f, 0.0f,
					  	 position.x, position.y + resolution.h, 0.0f, 0.0f 
						};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void UI::updatePosition(){
	if(parent != NULL){
		resolution.w = currScale.x * parent->getResolution().w / 100.0f;
		resolution.h = !currScale.y ? resolution.w : currScale.y * parent->getResolution().h / 100.0f;
		position.x   = (ratio.x != 0.0f? ratio.x * (parent->getResolution().w - resolution.w) / 100.0f: 0.0f) + parent->getPosition().x;
		position.y   = (ratio.y != 0.0f? ratio.y * (parent->getResolution().h - resolution.h) / 100.0f: 0.0f) + parent->getPosition().y;
	}
	else{
		SDL_GetWindowSize(window, &screenWidth, &screenHeight);	
		resolution.w = currScale.x * screenWidth / 100.0f;
		resolution.h = !currScale.y? resolution.w: currScale.y * screenHeight / 100.0f;
		position.x   = ratio.x * (screenWidth  - resolution.w) / 100.0f;
		position.y   = ratio.y * (screenHeight - resolution.h) / 100.0f;
	}
}

void UI::updateActiveRegion(SDL_Event* event){
	bool insideW = false;
	bool insideH = false;
	if(resetActiveRegion){
		float delta     = borderMin - position.y;
		float resultMin = (delta > 0.0f)? borderMin: position.y;

		delta = position.y + resolution.h - borderMax;
		float resultMax = delta > 0.0f? borderMax: position.y + resolution.h;   

		insideH = event->button.y < screenHeight - resultMin && event->button.y > screenHeight - resultMax; 
		insideW = event->button.x > position.x && event->button.x < position.x + resolution.w;
	}
	else{
		insideW = event->button.x > position.x && event->button.x < position.x + resolution.w;
		insideH = event->button.y > screenHeight - (position.y + resolution.h) && event->button.y < (screenHeight - (position.y + resolution.h)) + resolution.h;
	}
	inside = insideW && insideH;
}

void UI::hident(bool flag){
	this->hided = flag;
	if(!child.empty()){
		for(uint i = 0; i < child.size(); ++i){
			child[i]->hident(hided);
		}
	}
}

void UI::setParent(UI* object){
	this->parent = object;
	if(parent != NULL){
		this->hided = parent->hided;
		this->parent->setChild(this);
	}
	updatePosition();
	updateBuffer();
}

void UI::setChild(UI* object){
	child.push_back(object);
}

void UI::setResolution(float w, float h){
	this->resolution = vec2f((w == 0.0f? resolution.w: w), (h == 0.0? resolution.h: h));
}

void UI::setScale(float x, float y){
	currScale.x = x;
	currScale.y = y;
}

void UI::setRatio(float x, float y){
	ratio.x = x;
	ratio.y = y;
}

void UI::setName(std::string name, float x, float y, const char* fontName, float size){
	this->uiName     = name;
	this->fontFamily = fontName != NULL? "../assets/fonts/" + std::string(fontName): fontFamily;
	this->fontSize   = size;
	this->text       = Text(fontFamily.c_str(), fontSize);
	this->text.setPosition(x, y);
}

void UI::setPosition(float x, float y){
	this->position = vec2f(x, y);
}

void UI::summon(UI* object) {
	object->summoner(this);
	objectSummon = object;
}

void UI::summoner(UI* object) {
	objectSummoner = object;
}

UI* UI::getSummon() {
	return objectSummon;
}

UI* UI::getSummoner() {
	return objectSummoner;
}

UI* UI::getParent(){
	return parent;
}

std::vector<UI*>& UI::getChild(){
	return child;
}

bool UI::isHidden() const{
	return hided;
}

vec2f UI::getPosition(){
	return position;
}

vec2f UI::getResolution(){
	return resolution;
}

vec2f UI::getRatio(){
	return ratio;
}

vec2f UI::getScale(){
	return currScale;
}

Text& UI::getText(){
	return text;
}

//SECTION: Queue UI
void QueueUI::put(UI& object){
	//Atualiza certos parâmetros antes de colocar na fila
	object.updateUniforms();
	queue.push_back(&object);
}

void QueueUI::poll(SDL_Event* event){
	if(!queue.empty()){
		for(uint i = 0; i < queue.size(); ++i){
			if(queue[i]->enableUI && !UI::lockedUI)
				queue[i]->mouseEvent(event);
			queue[i]->windowEvent(event);
			queue[i]->keyboardEvent(event);
			if(!queue[i]->getChild().empty()){
				for(uint j = 0; j < queue[i]->getChild().size(); ++j){	
					queue[i]->getChild()[j]->windowEvent(event);
					if(!queue[i]->getChild()[j]->isHidden()){
						if(queue[i]->getChild()[j]->enableUI && !UI::lockedUI){
							queue[i]->getChild()[j]->mouseEvent(event);
						}
						if(!queue[i]->getChild()[j]->getChild().empty()){
							for(uint k = 0; k < queue[i]->getChild()[j]->getChild().size(); ++k){
								queue[i]->getChild()[j]->getChild()[k]->mouseEvent(event);
								queue[i]->getChild()[j]->getChild()[k]->windowEvent(event);
							}
						}
					}
				}
			}
		}
		//Scrollbar 
		for(uint i = 0; i < queue.size(); ++i){
			if(queue[i]->getSummon() != NULL){
				queue[i]->getSummon()->mouseEvent(event);
				for(uint j = 0; j < queue[i]->getSummon()->getChild().size(); ++j) {
					queue[i]->getSummon()->getChild()[j]->mouseEvent(event);
					if(!queue[i]->getSummon()->getChild()[j]->getChild().empty()){
						for(uint k = 0; k < queue[i]->getSummon()->getChild()[j]->getChild().size(); ++k){
							queue[i]->getSummon()->getChild()[j]->getChild()[k]->mouseEvent(event);
						}
					}

				}
			}
		}
	}
}

void QueueUI::free(){
	queue.clear();
}

void QueueUI::render(){
	if(!queue.empty()){
		for(uint i = 0; i < queue.size(); ++i){
			queue[i]->render();
		}
	}
}

//SECTION: Button UI
Button::Button(){
	hold 	   	  = false;
	applied	      = false;
	releaseTime   = 0;
	animationTime = 16;
}

Button::Button(float x, float y, vec2f scale, uint& programID, UI* container){
	hold 	   	  = false;
	applied	      = false;
	releaseTime   = 0;
	animationTime = 16;
	setRatio(x, y);
	setScale(scale.x, scale.y);
	setParent(container);
	updatePosition();
	genBuffer();
	setProgram(programID);
	highlight([&](void){
			setUniform3f(searchUniform3f("blend"), over);
		}
	);
	released([&](void){
			setUniform3f(searchUniform3f("blend"), out);
		}
	);
	setUniform1f("x", position.x);
	setUniform1f("y", position.y);
	setUniform1f("w", resolution.w);
	setUniform1f("h", resolution.h);
	setUniform1b("clipping", false);
	setUniform1f("borderMin", 0.0f);
	setUniform1f("borderMax", 0.0f);
}

Button::~Button(){
	//Nothing to do
}

void Button::updateUniforms(){
	unif1f[searchUniform1f("w")].value = resolution.w;
	unif1f[searchUniform1f("h")].value = resolution.h;
	unif1f[searchUniform1f("x")].value = position.x;
	unif1f[searchUniform1f("y")].value = position.y;
}

void Button::render(){
	if(!hided){
		releaseTime = applied? releaseTime + 16: 0;
		if(releaseTime == animationTime){
			applied = false;
			highlightFunction();
		} 
		glDisable(GL_DEPTH_TEST);
		glUseProgram(program);
		glBindVertexArray(VAO);
		initUnif1b();
		initUnifSampler2D();
		initUnif1f();
		initUnif3f();
		initUnifMat4fv();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);
		if(uiName != "UNKNOWNNAME"){
			float x = text.getPosition().x != 0.0f ? ((text.getPosition().x * (resolution.w - text.getSize(uiName).w)) / 100.0f) + position.x : position.x;
			float y = text.getPosition().y != 0.0f ? ((text.getPosition().y * (resolution.h - text.getSize(uiName).h)) / 100.0f) + position.y : position.y;
			// std::cout << uiName << ':' << x << '|' << y << std::endl;
			text.render(uiName, x, y, 1.0f);
		}
		glEnable(GL_DEPTH_TEST);
	}
}

void Button::terminate(){
	bool flag = getParent() != NULL? (getParent()->getParent() != NULL? true: false): false;
	if(flag){
		uint index = getParent()->getParent()->getSummoner()->searchUniform3f("blend");
		getParent()->getParent()->getSummoner()->setUniform3f(index, getParent()->getParent()->getSummoner()->out);
		getParent()->getParent()->getSummoner()->enableUI = !getParent()->getParent()->getSummoner()->enableUI;
		getParent()->getParent()->hident(!getParent()->getParent()->isHidden());
		UI::lockedUI = !UI::lockedUI;
	}
}

void Button::setAnimationTime(uint time){
	this->animationTime = time;
}

bool Button::isApplied(){
	return applied;
}

void Button::highlight(std::function<void(void)> func){
	highlightFunction = func;
}

void Button::pressed(std::function<void(void)> func){
	pressedFunction = func;
}

void Button::released(std::function<void(void)> func){
	releasedFunction = func;
}

void Button::keyboardEvent(SDL_Event* event){
	//Nothing to do
}

void Button::mouseEvent(SDL_Event* event){
	if(!hided){
		updateActiveRegion(event);
		if(event->type == SDL_MOUSEMOTION){
			if(inside && !hold){ 
				highlightFunction();
			}
			else if(!hold){
				releasedFunction();
			}
		}
		if(event->type == SDL_MOUSEBUTTONDOWN){
			if(event->button.button == SDL_BUTTON_LEFT){
				if(inside && !hold){ 
					hold = true;
					pressedFunction();
				}
			}
		}
		if(event->type == SDL_MOUSEBUTTONUP){
			if(event->button.button == SDL_BUTTON_LEFT){
				if(inside && hold){ 
					hold     = false;
					applied  = true;
					pressedFunction();
				}
				else{
					hold = false;
					releasedFunction();
				}
			}
		}
	}
}

void Button::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			updatePosition(); 
			updateBuffer();
			updateUniforms(); //Botões com bordas
		}
	}
}

//SECTION: SCROLLPANEL UI
Scroll::Scroll(){
	scroll = false;
}

Scroll::~Scroll(){
	//Nothing to do
}

void Scroll::updateUniforms(){
	//Nothing to do
}

void Scroll::keyboardEvent(SDL_Event* event){
	//Nothing to do
}

void Scroll::mouseEvent(SDL_Event* event){
	if(event->type == SDL_MOUSEWHEEL){
		if(scroll && objectSummoner != NULL){
			if(slide == VERTICAL){	
				if(event->wheel.y > 0){
					if(position.y < objectSummoner->getPosition().y)
						setPosition(position.x, position.y + (event->wheel.y * 4));
					else
						setPosition(position.x, objectSummoner->getPosition().y);
					updateBuffer();
					for(uint i = 0; i < child.size(); ++i){
						child[i]->updatePosition();
						child[i]->updateBuffer();
						child[i]->updateUniforms();
						child[i]->borderMin = objectSummoner->getPosition().y;
						child[i]->borderMax = objectSummoner->getPosition().y + objectSummoner->getResolution().h;
						child[i]->updateActiveRegion(event);
						if(!child[i]->getChild().empty()){
							for(uint j = 0; j < child[i]->getChild().size(); ++j){
								child[i]->getChild()[j]->updatePosition();
								child[i]->getChild()[j]->updateBuffer();
								child[i]->getChild()[j]->updateUniforms();
								child[i]->getChild()[j]->borderMin = objectSummoner->getPosition().y;
								child[i]->getChild()[j]->borderMax = objectSummoner->getPosition().y + objectSummoner->getResolution().h;
								child[i]->getChild()[j]->updateActiveRegion(event);
							}
						}
					}
				}
				else if(event->wheel.y < 0){
					if(position.y + resolution.h > objectSummoner->getPosition().y + objectSummoner->getResolution().h)
						setPosition(position.x, position.y - (event->wheel.y * -4));
					else
						setPosition(position.x, position.y - ((position.y + resolution.h) - (objectSummoner->getPosition().y + objectSummoner->getResolution().h)));
					updateBuffer();
					for(uint i = 0; i < child.size(); ++i){
						child[i]->updatePosition();
						child[i]->updateBuffer();
						child[i]->updateUniforms();
						child[i]->borderMin = objectSummoner->getPosition().y;
						child[i]->borderMax = objectSummoner->getPosition().y + objectSummoner->getResolution().h;
						child[i]->updateActiveRegion(event);
						if(!child[i]->getChild().empty()){
							for(uint j = 0; j < child[i]->getChild().size(); ++j){
								child[i]->getChild()[j]->updatePosition();
								child[i]->getChild()[j]->updateBuffer();
								child[i]->getChild()[j]->updateUniforms();
								child[i]->getChild()[j]->borderMin = objectSummoner->getPosition().y;
								child[i]->getChild()[j]->borderMax = objectSummoner->getPosition().y + objectSummoner->getResolution().h;
								child[i]->getChild()[j]->updateActiveRegion(event);
							}
						}
					}
				}
			}
		}
	}
	else{
		for(uint i = 0; i < child.size(); ++i){
			child[i]->hident(objectSummoner->isHidden());
			child[i]->updatePosition();
			child[i]->updateBuffer();
			child[i]->borderMin = slide? objectSummoner->getPosition().x: objectSummoner->getPosition().y;
			child[i]->borderMax = slide? objectSummoner->getPosition().x + objectSummoner->getResolution().w: objectSummoner->getPosition().y + objectSummoner->getResolution().h;
			child[i]->updateActiveRegion(event);
		}
	}
}

void Scroll::windowEvent(SDL_Event* event){
	//Nothing do to
}

void Scroll::render(){
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUnifSampler2D();
	initUnif1f();
	initUnif3f();
	initUnifMat4fv();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);	
	glEnable(GL_DEPTH_TEST);
}

void Scroll::enable(bool flag){
	scroll = flag;
}

bool Scroll::is(){
	return scroll;
}

void Scroll::roll(bool flag) {
	this->slide = flag;
}

void Scroll::save(vec2f coord){
	coordinates = coord;
}

vec2f Scroll::saved(){
	return coordinates;
}

//SECTION: Panel UI
Panel::Panel(){
	//Nothing to do
}

Panel::Panel(float x, float y, vec2f scale, uint& programID, UI* container){
	setRatio(x, y);
	setScale(scale.x, scale.y);
	updatePosition();
	genBuffer();
	setParent(container);
	setProgram(programID);

	//Decide entre usar bordas com gradientes ou uniformes
	setUniform1f("x", position.x);
	setUniform1f("y", position.y);
	setUniform1f("w", resolution.w);
	setUniform1f("h", resolution.h);
	setUniform1b("clipping", false);
	setUniform1f("borderMin", 0.0f);
	setUniform1f("borderMax", 0.0f);
	setUniform1f("opacity", 1.0f);
}

Panel::~Panel() {
	//Nothing to do
}

void Panel::bordered(vec3f color, float thickness, int mask, int style){
	setUniform3f("border", color);
	setUniform1f("thickness", thickness);
	setUniform1f("style", style);
	switch(mask){
		case UNIFORM:
			addTexture("../assets/ui/rampnull.jpg", "rampTex", 0, false, false);
			break;
		case GRADIENT:
			addTexture("../assets/ui/ramp.jpg", "rampTex", 0, false, false);
			break;
	}
}

void Panel::fill(vec3f color){
	setUniform3f("fill", color);
}

void Panel::opacity(float alpha){
	setUniform1f(searchUniform1f("opacity"), alpha);
}

void Panel::scrollbar(bool flag, vec2f scale){
	scroll.enable(flag);
	if(scroll.is()){	
		scroll.roll(VERTICAL);
		scroll.setRatio(ratio.x, ratio.y);
		scroll.setScale((100 * (resolution.w * scale.x / 100)) / screenWidth, scale.y);
		scroll.updatePosition();
		scroll.setPosition(position.x, position.y - (scroll.getResolution().h - resolution.h));			
		scroll.genBuffer();
		//As seguintes funções chamadas de SRW para scroll são para fins de debug
	#if DEBUG
		scroll.setProgram(program);
		scroll.addTexture("../assets/ui/rampnull.jpg", "rampTex", 0, false, false);
		scroll.setUniform1f("thickness", 0.0f);
		scroll.setUniform3f("fill", vec3f(0.0f, 0.0f, 0.0f));
		scroll.setUniform3f("border", vec3f(0.0f, 0.0f, 0.0f));
		scroll.setUniform1f("opacity", 1.0f);
		scroll.setUniform1f("x", 0.0f);
		scroll.setUniform1f("y", 0.0f);
		scroll.setUniform1f("w", 0.0f);
		scroll.setUniform1f("h", 0.0f);
	#endif
		scroll.hident(true);
		float minValue = position.y + unif1f[searchUniform1f("thickness")].value;
		float maxValue = position.y + resolution.h - unif1f[searchUniform1f("thickness")].value;
		for(uint i = 0; i < getChild().size(); ++i){
			child[i]->setParent(&scroll);
			scroll.getChild()[i]->setUniform1b(scroll.getChild()[i]->searchUniform1b("clipping"), true);		
			scroll.getChild()[i]->setUniform1f(scroll.getChild()[i]->searchUniform1f("borderMin"), minValue);	
			scroll.getChild()[i]->setUniform1f(scroll.getChild()[i]->searchUniform1f("borderMax"), maxValue);
			scroll.getChild()[i]->getText().clipping(true);
			scroll.getChild()[i]->getText().setBorderMax(maxValue);
			scroll.getChild()[i]->getText().setBorderMin(minValue);
			scroll.getChild()[i]->updatePosition();
			scroll.getChild()[i]->updateBuffer();
			scroll.getChild()[i]->updateUniforms();
			scroll.getChild()[i]->borderMin = minValue;
			scroll.getChild()[i]->borderMax = maxValue;
			scroll.getChild()[i]->resetActiveRegion = true;
			if(!scroll.getChild()[i]->getChild().empty()){
				for(uint j = 0; j < scroll.getChild()[i]->getChild().size(); ++j){
					scroll.getChild()[i]->getChild()[j]->setUniform1b(scroll.getChild()[i]->getChild()[j]->searchUniform1b("clipping"), true);
					scroll.getChild()[i]->getChild()[j]->setUniform1f(scroll.getChild()[i]->getChild()[j]->searchUniform1f("borderMin"), minValue + unif1f[searchUniform1f("thickness")].value);
					scroll.getChild()[i]->getChild()[j]->setUniform1f(scroll.getChild()[i]->getChild()[j]->searchUniform1f("borderMax"), maxValue - unif1f[searchUniform1f("thickness")].value);
					scroll.getChild()[i]->getChild()[j]->getText().clipping(true);
					scroll.getChild()[i]->getChild()[j]->getText().setBorderMax(maxValue);
					scroll.getChild()[i]->getChild()[j]->getText().setBorderMin(minValue);
					scroll.getChild()[i]->getChild()[j]->updatePosition();
					scroll.getChild()[i]->getChild()[j]->updateBuffer();
					scroll.getChild()[i]->getChild()[j]->updateUniforms();
					scroll.getChild()[i]->getChild()[j]->borderMin = minValue;
					scroll.getChild()[i]->getChild()[j]->borderMax = maxValue;
					scroll.getChild()[i]->getChild()[j]->resetActiveRegion = true;
				}
			}
		}	
		scroll.save(scroll.getPosition());
		getChild().clear();
		summon(&scroll);
	}
}

void Panel::updateUniforms(){
	unif1f[searchUniform1f("w")].value = resolution.w;
	unif1f[searchUniform1f("h")].value = resolution.h;
	unif1f[searchUniform1f("x")].value = position.x;
	unif1f[searchUniform1f("y")].value = position.y;
}

void Panel::resetCoordinates(){
	scroll.setPosition(scroll.saved().x, scroll.saved().y);
	scroll.updateBuffer();
	if(!scroll.getChild().empty()){
		for(uint i = 0; i < scroll.getChild().size(); ++i){
			scroll.getChild()[i]->updatePosition();
			scroll.getChild()[i]->updateBuffer();
			scroll.getChild()[i]->updateUniforms();
			if(!scroll.getChild()[i]->getChild().empty()){
				for(uint j = 0; j < scroll.getChild()[i]->getChild().size(); ++j){
					scroll.getChild()[i]->getChild()[j]->updatePosition();
					scroll.getChild()[i]->getChild()[j]->updateBuffer();
					scroll.getChild()[i]->getChild()[j]->updateUniforms();
					if(!scroll.getChild()[i]->getChild()[j]->getChild().empty()){
						for(uint k = 0; k < scroll.getChild()[i]->getChild()[j]->getChild().size(); ++k){
							scroll.getChild()[i]->getChild()[j]->getChild()[k]->updatePosition();
							scroll.getChild()[i]->getChild()[j]->getChild()[k]->updateBuffer();
							scroll.getChild()[i]->getChild()[j]->getChild()[k]->updateUniforms();
						}
					}
				}
			}
		}
	}
}

void Panel::render(){
	if(!hided){
		glDisable(GL_DEPTH_TEST);
		glUseProgram(program);
		glBindVertexArray(VAO);
		initUnif1b();
		initUnifSampler2D();
		initUnif1f();
		initUnif3f();
		initUnifMat4fv();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);
		if(uiName != "UNKNOWNNAME"){
			float x = text.getPosition().x != 0.0f ? (text.getPosition().x * (resolution.w - text.getSize(uiName).w) / 100) + position.x : position.x;
			float y = text.getPosition().y != 0.0f ? (text.getPosition().y * (resolution.h - text.getSize(uiName).h) / 100) + position.y : position.y;
			text.render(uiName, x, y, 1.0f);
		}
		glEnable(GL_DEPTH_TEST);
		if(scroll.is()){
			// scroll.render();
			//scrollbutton.render();
		}
	}
	else{
		if(scroll.is()){
			resetCoordinates();
		}
	}	
}

void Panel::keyboardEvent(SDL_Event* event){
	scroll.keyboardEvent(event);
}

void Panel::mouseEvent(SDL_Event* event){
	if(event->type == SDL_MOUSEWHEEL && !hided){
		scroll.mouseEvent(event);
		if(scroll.is()){
			//Do any thing
		}
	}
}

void Panel::windowEvent(SDL_Event* event){
	//vec2f previusDimensions = vec2f(currWidth, currHeight);
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			//fontSize = (currWidth * fontSize) / previusDimensions.w;
			//Text temp(fontFamily.c_str(), fontSize);
			//temp.setPosition(text.getPosition().x, text.getPosition().y);
			//text = temp;
			//text.setPosition(temp.getPosition().x, temp.getPosition().y);
			updatePosition();
			updateBuffer();
			updateUniforms();
			if(scroll.is()){
				scroll.setResolution((scroll.getScale().x * screenWidth) / 100.0f, (scroll.getScale().y * screenHeight / 100.0f)); 
				scroll.updateBuffer();
				for(uint i = 0; i < scroll.getChild().size(); ++i){
					scroll.getChild()[i]->setUniform1f(scroll.getChild()[i]->searchUniform1f("borderMin"), position.y + unif1f[searchUniform1f("thickness")].value);
					scroll.getChild()[i]->setUniform1f(scroll.getChild()[i]->searchUniform1f("borderMax"), position.y + resolution.h - unif1f[searchUniform1f("thickness")].value);
					scroll.getChild()[i]->getText().setBorderMin(position.y + unif1f[searchUniform1f("thickness")].value);
					scroll.getChild()[i]->getText().setBorderMax(position.y + resolution.h - unif1f[searchUniform1f("thickness")].value);
					scroll.getChild()[i]->updatePosition();
					scroll.getChild()[i]->updateBuffer();
					scroll.getChild()[i]->borderMin = position.y;
					scroll.getChild()[i]->borderMax = position.y + resolution.h;
					if(!scroll.getChild()[i]->getChild().empty()){
						for(uint j = 0; j < scroll.getChild()[i]->getChild().size(); ++j){
							scroll.getChild()[i]->getChild()[j]->setUniform1f(scroll.getChild()[i]->getChild()[j]->searchUniform1f("borderMin"), position.y + 1 + unif1f[searchUniform1f("thickness")].value);
							scroll.getChild()[i]->getChild()[j]->setUniform1f(scroll.getChild()[i]->getChild()[j]->searchUniform1f("borderMax"), position.y + resolution.h - 1 - unif1f[searchUniform1f("thickness")].value);
							scroll.getChild()[i]->getChild()[j]->getText().setBorderMin(position.y + unif1f[searchUniform1f("thickness")].value);
							scroll.getChild()[i]->getChild()[j]->getText().setBorderMax(position.y + resolution.h - unif1f[searchUniform1f("thickness")].value);
							scroll.getChild()[i]->getChild()[j]->updatePosition();
							scroll.getChild()[i]->getChild()[j]->updateBuffer();
							scroll.getChild()[i]->getChild()[j]->borderMin = position.y;
							scroll.getChild()[i]->getChild()[j]->borderMax = position.y + resolution.h;
						}
					}
				}
			}
		}
	}
}

std::vector<SpriteSheet*> SpriteSheet::instances;

SpriteSheet::SpriteSheet(const char* pathSpriteSheet, float x, float y, float w, float h, uint division, bool flag){
	SpriteSheet::instances.push_back(this);
	FOWARD 		  = false;
	BACKFOWARD 	  = false;
	RIGHT 		  = false;
	LEFT 		  = false;
	elapsed_time  = 0;
	current_time  = 0;
	frame_time 	  = 0;
	current_frame = 1;
	end_at 		  = 0;
	loop    	  = flag;
	iteratorA     = 0;
	iteratorB 	  = 0;
	hided 		  = false;
	edgeLeft   	  = true;
	edgeRight  	  = true;
	edgeTop    	  = true;
	edgeBottom 	  = true;
	genShader("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_sprite_sheet.glsl");
	setUniformMat4fv("uiMatrix", &UI::uiMatrix);
	uint width, height;
	addTexture(pathSpriteSheet, "tex", 0, false, false, &width, &height);
	sampler2D.pop_back();
	setRatio(x, y);
	setScale(w, h);
	updatePosition();
	split 	   = division;
	split_size = (width > height ? width : height) / split;
	clip.resize(split);
	//Just functionaly for horizontal sprite sheet
	for(uint i = 0; i <= split; ++i){
		clip[i] = (float)(split_size * i) / (float)(width > height ? width : height);
	}
	float vertices[] = {
				 position.x, position.y + resolution.h, clip[0], 0.0f,
				 position.x, position.y, clip[0], 1.0f,
				 position.x + resolution.w, position.y, clip[1], 1.0f,
				 position.x + resolution.w, position.y, clip[1], 1.0f,
				 position.x + resolution.w, position.y + resolution.h, clip[1], 0.0f,
				 position.x, position.y + resolution.h, clip[0], 0.0f
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glBindVertexArray(0);
}

SpriteSheet::~SpriteSheet(){
	clip.clear();
	frame.clear();
}

void SpriteSheet::addSpriteSheet(const char* pathSpriteSheet, SDL_Scancode scancode){
	addTexture(pathSpriteSheet, "tex", 0, false, false, NULL, NULL);
	key.push_back(scancode);
}

void SpriteSheet::frameTime(uint ms){
	this->frame_time = ms;
	for (uint i = 1; i < split + 1; ++i) {
		frame[i] = ms;
		end_at += frame[i];
	}
}

void SpriteSheet::frameTime(uint frame_id, uint ms){
	frame[frame_id] = ms;
	end_at = 0;
	for (uint i = 1; i < split + 1; ++i) {
		end_at += frame[i];
	}
}

void SpriteSheet::setRatio(float x, float y){
	this->ratio = vec2f(x, y);
}

void SpriteSheet::setScale(float w, float h){
	this->scale = vec2f(w, h);
}

void SpriteSheet::setPosition(float x, float y){
	this->position = vec2f(x, y);
}

void SpriteSheet::setResolution(float w, float h){
	this->resolution = vec2f(w, h);
}

vec2f SpriteSheet::getRatio(){
	return ratio;
}

vec2f SpriteSheet::getScale(){
	return scale;
}

vec2f SpriteSheet::getPosition(){
	return position;
}

vec2f SpriteSheet::getResolution(){
	return resolution;
}

void SpriteSheet::updatePosition(){
	SDL_GetWindowSize(window, &screenWidth, &screenHeight);	
	resolution.w = scale.x * screenWidth / 100.0f;
	resolution.h = !scale.y? resolution.w: scale.y * screenHeight / 100.0f;
	position.x   = ratio.x * (screenWidth  - resolution.w) / 100.0f;
	position.y   = ratio.y * (screenHeight - resolution.h) / 100.0f;
}

void SpriteSheet::updateBuffer(){
	float vertices[] = {
		position.x, position.y + resolution.h, clip[iteratorA], 0.0f,
		position.x, position.y, clip[iteratorA], 1.0f,
		position.x + resolution.w, position.y, clip[iteratorB], 1.0f,
		position.x + resolution.w, position.y, clip[iteratorB], 1.0f,
		position.x + resolution.w, position.y + resolution.h, clip[iteratorB], 0.0f,
		position.x, position.y + resolution.h, clip[iteratorA], 0.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool SpriteSheet::collision(){
	ratio.x += velocity.x;
	if(ratio.x < 0.0f || ratio.x > 100.0f)
		ratio.x -= velocity.x; 
	ratio.y += velocity.y;
	if(ratio.y < 0.0f || ratio.y > 100.0f)
		ratio.y -= velocity.y;
	updatePosition();
	for(uint i = 0; i < SpriteSheet::instances.size(); ++i){
		//"i" instance box collisor bottom
		i_h_box_res      = SpriteSheet::instances[i]->getResolution();
		i_h_box_res 		   = vec2f(i_h_box_res.w / 1.5f, i_h_box_res.h / 4.0f);
		i_h_b_box_left   = SpriteSheet::instances[i]->getPosition().x + ((SpriteSheet::instances[i]->getResolution().w - i_h_box_res.w) / 2.0f);
		i_h_b_box_right  = i_h_b_box_left + i_h_box_res.w;
		i_h_b_box_top    = SpriteSheet::instances[i]->getPosition().y + i_h_box_res.h;
		i_h_b_box_bottom = SpriteSheet::instances[i]->getPosition().y;

		//"i" instance box colissor top
		i_h_t_box_top    = SpriteSheet::instances[i]->getPosition().y + SpriteSheet::instances[i]->getResolution().h;
		i_h_t_box_bottom = i_h_t_box_top - i_h_box_res.h;

		//"i" instance box collisor right
		i_v_box_res      = SpriteSheet::instances[i]->getResolution();
		i_v_box_res 	       = vec2f(i_v_box_res.w / 6.0f, i_v_box_res.h / 2.0f);
		i_v_r_box_right  = SpriteSheet::instances[i]->getPosition().x + SpriteSheet::instances[i]->getResolution().w;
		i_v_r_box_left   = i_v_r_box_right - i_v_box_res.w;
		i_v_r_box_top    = SpriteSheet::instances[i]->getPosition().y + SpriteSheet::instances[i]->getResolution().h - (i_v_box_res.h / 2.0f);
		i_v_r_box_bottom = i_v_r_box_top - i_v_box_res.h;

		//"i" instance box collisor left
		i_v_l_box_left   = SpriteSheet::instances[i]->getPosition().x;
		i_v_l_box_right  = i_v_l_box_left + i_v_box_res.w;
		i_v_l_box_top    = SpriteSheet::instances[i]->getPosition().y + SpriteSheet::instances[i]->getResolution().h - (i_v_box_res.h / 2.0f);
		i_v_l_box_bottom = i_v_l_box_top - i_v_box_res.h;

		//"player" instance collisor bottom
		p_h_box_res      = resolution;
		p_h_box_res 		   = vec2f(p_h_box_res.w / 1.5f, p_h_box_res.h / 4.0f);
		p_h_b_box_left   = position.x + ((resolution.w - p_h_box_res.w) / 2.0f); 
		p_h_b_box_right  = p_h_b_box_left + p_h_box_res.w;
		p_h_b_box_top    = position.y + p_h_box_res.h;
		p_h_b_box_bottom = position.y;

		//"player" instance collisor top
		p_h_t_box_top    = position.y + resolution.h;
		p_h_t_box_bottom = p_h_t_box_top - p_h_box_res.h;

		//"player" instance collisor left
		p_v_box_res      = resolution;
		p_v_box_res			   = vec2f(p_v_box_res.w / 6.0f, p_v_box_res.h / 2.0f);
		p_v_l_box_left   = position.x;
		p_v_l_box_right  = p_v_l_box_left + p_v_box_res.w;
		p_v_l_box_top    = position.y + resolution.h - (p_v_box_res.h / 2.0f);
		p_v_l_box_bottom = p_v_l_box_top - p_v_box_res.h;

		//"player" instance collisor right
		p_v_r_box_right  = position.x + resolution.w;
		p_v_r_box_left   = p_v_r_box_right - p_v_box_res.w;
		p_v_r_box_top    = position.y + resolution.h - (p_v_box_res.h / 2.0f);
		p_v_r_box_bottom = p_v_r_box_top - p_v_box_res.h;

		//"player" left collision of the "i"
		edgeRight = true;
		if(p_v_r_box_left >= i_v_l_box_right){
			edgeRight = false;
		}
		if(p_v_r_box_right <= i_v_l_box_left){
			edgeRight = false;
		}
		if(p_v_r_box_bottom >= i_v_l_box_top){
			edgeRight = false;
		}
		if(p_v_r_box_top <= i_v_l_box_bottom){
			edgeRight = false;
		}

		//"player" right collision of the "i"
		edgeLeft = true;
		if(p_v_l_box_left >= i_v_r_box_right){
			edgeLeft = false;
		}
		if(p_v_l_box_right <= i_v_r_box_left){
			edgeLeft = false;
		}
		if(p_v_l_box_bottom >= i_v_r_box_top){
			edgeLeft = false;
		}
		if(p_v_l_box_top <= i_v_r_box_bottom){
			edgeLeft = false;
		}
		
		//"player" bottom collision of the "i"
		edgeTop = true;
		if(p_h_t_box_bottom >= i_h_b_box_top){
			edgeTop = false;
		}
		if(p_h_t_box_top <= i_h_b_box_bottom){
			edgeTop = false;
		}
		if(p_h_b_box_left >= i_h_b_box_right){
			edgeTop = false;
		}
		if(p_h_b_box_right <= i_h_b_box_left){
			edgeTop = false;
		}

		//"player" top collision of the "i"
		edgeBottom = true;
		if(p_h_b_box_bottom >= i_h_t_box_top){
			edgeBottom = false;
		}
		if(p_h_b_box_top <= i_h_t_box_bottom){
			edgeBottom = false;
		}
		if(p_h_b_box_left >= i_h_b_box_right){
			edgeBottom = false;
		}
		if(p_h_b_box_right <= i_h_b_box_left){
			edgeBottom = false;
		}

		//border detected
		if(edgeRight){
			ratio.x -= velocity.x;
		}
		if(edgeLeft){
			ratio.x -= velocity.x;
		}
		if(edgeTop){
			ratio.y -= velocity.y;
		}
		if(edgeBottom){
			ratio.y -= velocity.y;
		}
		updatePosition();

		//show edges
		// rect.render(p_v_l_box_left, p_v_l_box_bottom, p_v_box_res.w, p_v_box_res.h);
		// rect.render(p_v_r_box_left, p_v_r_box_bottom, p_v_box_res.w, p_v_box_res.h);
		// rect.render(p_h_b_box_left, p_h_t_box_bottom, p_h_box_res.w, p_h_box_res.h);
		// rect.render(p_h_b_box_left, p_h_b_box_bottom, p_h_box_res.w, p_h_box_res.h);
		// rect.render(i_v_l_box_left, i_v_l_box_bottom, i_v_box_res.w, i_v_box_res.h);
		// rect.render(i_v_r_box_left, i_v_r_box_bottom, i_v_box_res.w, i_v_box_res.h);
		// rect.render(i_h_b_box_left, i_h_t_box_bottom, i_h_box_res.w, i_h_box_res.h);
		// rect.render(i_h_b_box_left, i_h_b_box_bottom, i_h_box_res.w, i_h_box_res.h);
		if((edgeRight || edgeLeft) || (edgeBottom || edgeTop))
			break;
	}
}

void SpriteSheet::render() {
	if(!hided){
		current_time += 16;
		std::map<uint, uint>::iterator it;
		it = frame.find(current_frame);
		uint time_per_frame = it != frame.end() ? frame.find(current_frame)->second : frame_time;
		if(current_time == time_per_frame){
			elapsed_time += time_per_frame;
			current_frame++;
			if(!loop){
				iteratorA = iteratorA < split - 1 ? iteratorA + 1 : split - 1;
				iteratorB = iteratorA + 1;
			}
		}
		if(loop && iteratorA < split)
			iteratorB = iteratorA == 0? 1: iteratorA + 1;
		float vertices[] = {
					 position.x, position.y + resolution.h, clip[iteratorA], 0.0f,
					 position.x, position.y, clip[iteratorA], 1.0f,
					 position.x + resolution.w, position.y, clip[iteratorB], 1.0f,
					 position.x + resolution.w, position.y, clip[iteratorB], 1.0f,
					 position.x + resolution.w, position.y + resolution.h, clip[iteratorB], 0.0f,
					 position.x, position.y + resolution.h, clip[iteratorA], 0.0f
			};
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		if(current_time == time_per_frame){
			if(loop){
				iteratorA = iteratorA < split - 1? iteratorA + 1: split - 1;
			}
			current_time = 0;
		}
		if(!loop){
			hided = elapsed_time == end_at ? true : false;
		}
		else{
			if(elapsed_time == end_at){
				current_frame = 0;
				current_time = 0;
				elapsed_time = 0;
				iteratorA = 0;
				iteratorB = 0;
			}
		}
		glUseProgram(program);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		if(sampler2D.size() > 1){
			if(FOWARD){
				glBindTexture(GL_TEXTURE_2D, sampler2D[1].texID);
			}
			if(BACKFOWARD){
				glBindTexture(GL_TEXTURE_2D, sampler2D[2].texID);
			}
			if(RIGHT){
				glBindTexture(GL_TEXTURE_2D, sampler2D[3].texID);
			}
			if(LEFT){
				glBindTexture(GL_TEXTURE_2D, sampler2D[4].texID);
			}
			if(FOWARD && RIGHT){
				glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);
			}
			if(FOWARD && LEFT){
				glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);	
			}
			if(BACKFOWARD && RIGHT){
				glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);
			}
			if(BACKFOWARD && LEFT){
				glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);	
			}
			if(FOWARD == false && BACKFOWARD == false && RIGHT == false && LEFT == false){
				glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);
			}
		}
		else{
			glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);
		}
		glUniform1i(glGetUniformLocation(program, "tex"), 0);
		initUnifMat4fv();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}

void SpriteSheet::keyboardEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN && event->key.repeat == 0){
		if(key[0] == event->key.keysym.scancode){
			velocity.y += offset_per_frame;
			FOWARD = true;
		}
		if(key[1] == event->key.keysym.scancode){
			velocity.y -= offset_per_frame;
			BACKFOWARD = true;
		}
		if(key[2] == event->key.keysym.scancode){
			velocity.x += offset_per_frame;
			RIGHT = true;
		}
		if(key[3] == event->key.keysym.scancode){
			velocity.x -= offset_per_frame;
			LEFT = true;
		}
	}
	else if(event->type == SDL_KEYUP && event->key.repeat == 0){
		if(key[0] == event->key.keysym.scancode){
			velocity.y -= offset_per_frame;
			FOWARD = false;
		}
		if(key[1] == event->key.keysym.scancode){
			velocity.y += offset_per_frame;
			BACKFOWARD = false;
		}
		if(key[2] == event->key.keysym.scancode){
			velocity.x -= offset_per_frame;
			RIGHT = false;
		}
		if(key[3] == event->key.keysym.scancode){
			velocity.x += offset_per_frame;
			LEFT = false;
		}
	}
}

void SpriteSheet::mouseEvent(SDL_Event* event){

}

void SpriteSheet::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			updatePosition();
			updateBuffer();
		}
	}
}

SDL_AudioSpec audio;
Uint8* audio_buf;
Uint32 audio_len;
SDL_AudioDeviceID deviceID;
SDL_Thread* audio_thread;

int Sound::audioThread(void* ptr){
	SDL_Delay(2048);
	if(!SDL_QueueAudio(deviceID, audio_buf, audio_len))
		SDL_PauseAudioDevice(deviceID, 0);
	switch (SDL_GetAudioDeviceStatus(deviceID)){
	case SDL_AUDIO_STOPPED:
		std::cout << "stoped\n";
		break;
	case SDL_AUDIO_PLAYING:
		std::cout << "playng\n";
		break;
	case SDL_AUDIO_PAUSED:
		std::cout << "paused\n";
		break;
	default:
		std::cout << "unknown\n";
		break;
	}
	SDL_FreeWAV(audio_buf);
	SDL_DetachThread(audio_thread);
}

Sound::Sound(const char* path){
	if(SDL_LoadWAV(path, &audio, &audio_buf, &audio_len) == NULL){
		std::cerr << "Failed to load file " << path << std::endl;
		exit(1);
	}
	deviceID = SDL_OpenAudioDevice(NULL, 0, &audio, NULL, 0);
	if(deviceID == 0)
		std::cerr << "Failed do open device! SDL Error: " << SDL_GetError() << std::endl;
}

void Sound::execute(const char* name, int callback(void*)){
	int thread_return_value = 0;
	callback = &Sound::audioThread;
	audio_thread = SDL_CreateThread(callback, name, (void*)NULL);
	if(audio_thread == NULL)
		std::cerr << "SDL_CreateThread failed! SDL Error: " << SDL_GetError() << std::endl;
}