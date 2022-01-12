#include "../headers/objattrib/Material.hpp"


Material::Material(){
	setName("UNKNOWN");
	setAmbient(0.0f, 0.0f, 0.0f);
	setDiffuse(0.0f, 0.0f, 0.0f);
	setSpecular(0.0f, 0.0f, 0.0f);
	setShininess(0.0f);
}

Material::Material(const char* name, vec3f ambient, vec3f diffuse, vec3f specular, float shine){
	setName(name);
	setAmbient(ambient.x, ambient.x, ambient.x);
	setDiffuse(diffuse.x, diffuse.y, diffuse.z);
	setSpecular(specular.x, specular.y, specular.z);
	setShininess(shine);
}

void Material::setName(const char* n){
	this->name = n;
}

void Material::setAmbient(float red, float green, float blue){
	ambient.x = red;
	ambient.y = green;
	ambient.z = blue;
}

void Material::setDiffuse(float red, float green, float blue){
	diffuse.x = red;
	diffuse.y = green;
	diffuse.z = blue;
}

void Material::setSpecular(float red, float green, float blue){
	specular.x = red;
	specular.y = green;
	specular.z = blue;
}

void Material::setShininess(float value){
	shininess = value;
}

const char* Material::getName() const{
	return name;
}

vec3f Material::getAmbient() const{
	return ambient;
}

vec3f Material::getDiffuse() const{
	return diffuse;
}

vec3f Material::getSpecular() const{
	return specular;
}

float Material::getShininess() const{
	return shininess;
}

void Material::show(){
	std::cout << std::setw(10) << "Material Name: " << getName() << std::endl;
	getAmbient().show("Ambient");
	getDiffuse().show("Diffuse");
	getSpecular().show("Specular");
	std::cout << std::setw(12)  << "Shininess: "; 
	std::cout << std::setw(10)  << getShininess()  << std::endl;
}