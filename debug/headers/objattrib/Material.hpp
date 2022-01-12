#ifndef MATERIAL_H
#define MATERIAL_H
#include "../math/vec3.hpp"

class Material{
public:
	Material();
	Material(const char* name, vec3f ambient, vec3f diffuse, vec3f specular, float shine);
	void setName(const char* n);
	void setAmbient(float red, float green, float blue);
	void setDiffuse(float red, float green, float blue);
	void setSpecular(float red, float green, float blue);
	void setShininess(float value);
	const char* getName() const;
	vec3f getAmbient() const;
	vec3f getDiffuse() const;
	vec3f getSpecular() const; 
	float getShininess() const;
	void show();
private:
	const char* name;
	vec3f ambient;
	vec3f diffuse;
	vec3f specular;
	float shininess;
};
#endif