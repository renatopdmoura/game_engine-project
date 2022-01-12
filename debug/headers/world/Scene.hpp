/*
Descrição:
- Classe que armazena elementos de influência global como luzes
sobre objetos na cena atual.
*/
#ifndef SCENE_H
#define SCENE_H
#ifndef WRITE_IN_SHADER 
#define WRITE_IN_SHADER 0
#endif

#include "../math/vec3.hpp"
#include "../world/Light.hpp"
#include <fstream>
#include <vector>

class Scene{
public:
	void addLight(Light& light);
	void setWorldLight(Light& light);
	std::vector<Light*> getLights();
	void genLightInstances();
	Light getWorldLight();
	bool isSpotLight(unsigned int index) const;
	unsigned int searchLightByName(std::string keyWord) const;
private:
	std::vector<Light*> lights;
	Light* worldLight;
};
#endif