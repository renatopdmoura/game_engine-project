#include "../headers/world/Scene.hpp"

void Scene::addLight(Light& light){
	this->lights.push_back(&light);
}

void Scene::genLightInstances(){
	std::string pointLightSize("#version 460 \n#define NR_POINT_LIGHTS " + std::to_string(lights.size()) + ";\n");
	std::fstream file_fs_shader;		
	file_fs_shader.open("../shaders/fs_texture.glsl.c", std::ios_base::in | std::ios_base::binary);
	file_fs_shader.seekg(0, file_fs_shader.end);
	int length = file_fs_shader.tellg();
	file_fs_shader.seekg(0, file_fs_shader.beg);
	char* currentContent = new char[length];
	file_fs_shader.read(currentContent, length);
	file_fs_shader.close();

	file_fs_shader.open("../shaders/fs_texture.glsl.c", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	file_fs_shader.write(pointLightSize.c_str(), pointLightSize.size());
	file_fs_shader.write(currentContent, length);
	delete[] currentContent;
	file_fs_shader.close();
}

void Scene::setWorldLight(Light& light){
	this->worldLight = &light;
}

std::vector<Light*> Scene::getLights(){
	return lights;
}

Light Scene::getWorldLight(){
	return *worldLight;
}

bool Scene::isSpotLight(unsigned int index) const{
	return lights[index]->getType();
}

unsigned int Scene::searchLightByName(std::string keyWord) const{
		unsigned int i = 0;
		while(i < lights.size() && keyWord != lights[i]->getName()){
			++i;
		}
		if(keyWord == lights[i]->getName())
			return i;
		else
			std::cout << "Light <" << keyWord << "> no found! Returned index 0." << std::endl;
	return 0;
}