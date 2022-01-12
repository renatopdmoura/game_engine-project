#ifndef STRUCT_H
#define STRUCT_H

// #define STB_IMAGE_IMPLEMENTATION
// #include "stbi/stb_image.h"

#include "vec3.hpp"
#include "Scene.hpp"
#include "Material.hpp"
#include <vector>

struct Texture{
	Texture(){
		id     = -1;
		width  = 0;
		height = 0;
		data   = NULL;
	}
	inline void loadIMG(const char* texpath){
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		// data = stbi_load(texpath, &width, &height, &channels, 0);
		channels = channels == 4? channels + 1: channels == 3? channels + 1: channels == 2? channels - 1: 0; 
		if(data){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED + channels, width, height, 0, GL_RED + channels, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else{
			std::cout << "Failed to load texture: " << texpath << std::endl;
		}
		// stbi_image_free(data);	
	}
	inline void generateCubeMap(std::vector<std::string> path){
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for(unsigned i = 0; i < path.size(); ++i){
			// data = stbi_load(path[i].c_str(), &width, &height, &channels, 0);
			if(data){
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else{
				std::cout << "Failed to load texture: " << path[i] << std::endl;
			}
		}
		// stbi_image_free(data);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
	}
	GLuint id;
	int width;
	int height;
	int channels;
	unsigned char* data;
};

/*
- Obs.:Quando não necessidade de criar shaders exclusivos para objetos e/ou mais de um 
objeto compartilha os mesmos recursos de sombreamento, gere um identificador de programa
único.
*/

GLuint genShader(const char* vertexPath, const char* fragmentPath){
	GLuint program;
	createProgram(program);
	createShader(program, GL_VERTEX_SHADER,   readShaderSource(vertexPath));
	createShader(program, GL_FRAGMENT_SHADER, readShaderSource(fragmentPath));
	linkProgram(program);
	return program;
}

struct Shader{
	//Nested Structs
	struct Uniform3fv{
		Uniform3fv(const char* name, vec3f parm){
			uniformName = name;
			value = parm;
		}
		const char* uniformName;
		vec3f value;
	};
	struct Uniform1f{
		Uniform1f(const char* name, float parm){
			uniformName = name;
			value = parm;
		}
		const char* uniformName;
		float value;
	};
	struct Uniform1i{
		Uniform1i(const char* name, unsigned int parm){
			uniformName = name;
			value = parm;
		}
		const char* uniformName;
		unsigned int value;
	};
	//Basic functions
	Shader(){
		//Ainda não sei como inicializar na LoadOBJ
		isCubemap = false;
	}
	Shader(const char* vs_path, const char* fs_path, Scene* parm = NULL){
		scene = parm;
		create(vs_path, fs_path);
		isCubemap = false;
	}
	void free(){
		glDeleteProgram(program);
		textureID.clear();
		uniform1i.clear();
		uniform1f.clear();
		uniform3fv.clear();
	}
	void initUniforms(){
		if(!uniform1i.empty()){
			for(unsigned index = 0; index < uniform1i.size(); ++index){
				glActiveTexture(GL_TEXTURE0 + uniform1i[index].value);
				if(isCubemap)
					glBindTexture(GL_TEXTURE_CUBE_MAP, textureID[index]);
				else{
					glBindTexture(GL_TEXTURE_2D, textureID[index]);
				}
				glUniform1i(glGetUniformLocation(program, uniform1i[index].uniformName), uniform1i[index].value);
			}
		}
		if(!uniform3fv.empty()){
			for(unsigned index = 0; index < uniform3fv.size(); ++index){
				glUniform3fv(glGetUniformLocation(program, uniform3fv[index].uniformName), 1, uniform3fv[index].value.address());
			}
		}
		if(!uniform1f.empty()){
			for(unsigned index = 0; index < uniform1f.size(); ++index){
				glUniform1f(glGetUniformLocation(program, uniform1f[index].uniformName), uniform1f[index].value);
			}
		}
		renderLights();
	}
	void fsVersion(const char* fs_path){
		std::ifstream file(fs_path, std::ios_base::binary);
		if(!file.is_open()){
			std::cerr << "Failed to open file <" << fs_path << ">" << std::endl;
		}
		else{
			char code = '\0';
			std::filebuf* buffer = file.rdbuf();
			while(code != EOF){
				code = buffer->sbumpc();
				if(code == 'T' && buffer->sgetc() == ':'){
					char str[20];
					file.getline(str, 20, '\n');
					fragmentShaderVersion.clear();
					fragmentShaderVersion = str;
					fragmentShaderVersion.erase(0, 2);
					code = EOF;
				}
			}
		}
		file.close();
	}
	void create(const char* vs_path, const char* fs_path){
		fsVersion(fs_path);
		createProgram(program);
		createShader(program, GL_VERTEX_SHADER,   readShaderSource(vs_path));
		createShader(program, GL_FRAGMENT_SHADER, readShaderSource(fs_path));
		linkProgram(program);
	}
	void setUniform3fv(const char* uniformName, vec3f value){
		Uniform3fv temp(uniformName, value);
		uniform3fv.push_back(temp);
	}
	void setUniform1f(const char* uniformName, float value){
		Uniform1f temp(uniformName, value);
		uniform1f.push_back(temp);
	}
	void setUniform1i(const char* uniformName, unsigned int value){
		Uniform1i temp(uniformName, value);
		uniform1i.push_back(temp);
	}
	void setMaterial(Material* material){
		uniform3fv.clear();
		uniform1f.clear();
		if(fragmentShaderVersion == "OBJ_USE_MATERIAL"){
			setUniform3fv("material.ambient", material->getAmbient());
			setUniform3fv("material.diffuse", material->getDiffuse());
			setUniform3fv("material.specular", material->getSpecular());
			setUniform1f("material.shininess", material->getShininess());
		}
		else if(fragmentShaderVersion == "OBJ_USE_TEXTURE"){
			std::cerr << "WARNING::The shader define <" << fragmentShaderVersion << "> defines samplers. Do not supply materials for a sampling shader." << std::endl;
		}
	}
	void addTexture(const char* texpath, const char* uniformName, unsigned int value){
		Texture tex;
		tex.loadIMG(texpath);
		textureID.push_back(tex.id);
		setUniform1i(uniformName, value);
	}
	void addTexFromFramebuffer(unsigned int buffer, const char* uniformName, unsigned int value){
		textureID.push_back(buffer);
		setUniform1i(uniformName, value);
	}
	void addCubeMap(std::vector<std::string> path, const char* uniformName, unsigned int value){
		setUniform1i(uniformName, value);
		std::vector<Texture> tex;
		tex.resize(uniform1i.size());			
		for(unsigned i = 0; i < tex.size(); ++i){
			glGenTextures(1, &tex[i].id);
			tex[i].generateCubeMap(path);
			textureID.push_back(tex[i].id);
		}
		isCubemap = true;
	}
	void renderLights(){
		// for(unsigned i = 0; i < scene->getLights().size(); i++){
		// 	std::string index = std::to_string(i);
		// 	glUniform3fv(glGetUniformLocation(program, ("pointLight["+ index +"].position").c_str()), 1, scene->getLights()[i].getPosition().address());
		// 	glUniform3fv(glGetUniformLocation(program, ("pointLight["+ index +"].ambient").c_str()),  1, scene->getLights()[i].getAmbient().address());
		// 	glUniform3fv(glGetUniformLocation(program, ("pointLight["+ index +"].diffuse").c_str()),  1, scene->getLights()[i].getDiffuse().address());
		// 	glUniform3fv(glGetUniformLocation(program, ("pointLight["+ index +"].specular").c_str()), 1, scene->getLights()[i].getSpecular().address());
		// 	glUniform1f(glGetUniformLocation(program, ("pointLight["+ index +"].constant").c_str()), scene->getLights()[i].getConstantValue());
		// 	glUniform1f(glGetUniformLocation(program, ("pointLight["+ index +"].linear").c_str()), scene->getLights()[i].getLinearValue());
		// 	glUniform1f(glGetUniformLocation(program, ("pointLight["+ index +"].quadratic").c_str()), scene->getLights()[i].getQuadraticValue());
		// 	if(scene->isSpotLight(i)){
		// 		glUniform1f(glGetUniformLocation(program, ("pointLight["+ index +"].cosInner").c_str()), scene->getLights()[i].getCosInner());
		// 		glUniform1f(glGetUniformLocation(program, ("pointLight["+ index +"].cosOuter").c_str()), scene->getLights()[i].getCosOuter());
		// 	}
		// }
		// glUniform3fv(glGetUniformLocation(program, "directionalLight.direction"), 1, scene->getWorldLight().getDirection().address());
		// glUniform3fv(glGetUniformLocation(program, "directionalLight.ambient"), 1, scene->getWorldLight().getAmbient().address());
		// glUniform3fv(glGetUniformLocation(program, "directionalLight.diffuse"), 1, scene->getWorldLight().getDiffuse().address());
		// glUniform3fv(glGetUniformLocation(program, "directionalLight.specular"), 1, scene->getWorldLight().getSpecular().address());
	}
	void setVertexAttribPointer(){
		GLint positionAttribLocation = glGetAttribLocation(program, "position");
		if(positionAttribLocation == -1){
			std::cerr << "vertexPosition is not a valid glsl program variable or is not active!" << std::endl;
		}
		GLint normalAttribLocation = glGetAttribLocation(program, "normal");
		if(normalAttribLocation == -1){
			std::cerr << "normal is not a valid glsl program variable or is not active!" << std::endl;
		}
		GLint uvcoordAttribLocation = glGetAttribLocation(program, "uvcoord");
		if(uvcoordAttribLocation == -1){
			std::cerr << "uvcoord is not a valid glsl program variable or is not active!" << std::endl;
		}

		size_t offset_position = 0;
		size_t offset_normal   = sizeof(GLfloat) * 3;
		size_t offset_texture  = sizeof(GLfloat) * 6;

		glEnableVertexAttribArray(positionAttribLocation);
		glEnableVertexAttribArray(normalAttribLocation);
		glEnableVertexAttribArray(uvcoordAttribLocation);
		
		glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, reinterpret_cast<void*>(offset_position));
		glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, reinterpret_cast<void*>(offset_normal));
		glVertexAttribPointer(uvcoordAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, reinterpret_cast<void*>(offset_texture)); 
	}
	//Operators overloaded
	void operator=(Shader& shader){
		program               = shader.program;
		textureID             = shader.textureID;
		isCubemap    	      = shader.isCubemap;
		uniform1i             = shader.uniform1i;
		uniform1f             = shader.uniform1f;
		uniform3fv            = shader.uniform3fv;
		scene                 = shader.scene;
		fragmentShaderVersion = shader.fragmentShaderVersion;
	}
//private:
	GLuint program;
	std::vector<Uniform3fv> uniform3fv;
	std::vector<Uniform1f>  uniform1f;
	std::vector<Uniform1i>  uniform1i;
	std::vector<GLint> textureID;
	Scene* scene;
	bool isCubemap;
	std::string fragmentShaderVersion;
};
#endif