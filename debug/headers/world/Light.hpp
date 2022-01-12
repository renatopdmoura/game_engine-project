#ifndef LIGHT_H
#define LIGHT_H
#include "../math/vec3.hpp"
#include <math.h>

class Light{
public:
	Light();
	Light(const char* n, vec3f direction, vec3f position, vec3f ambient, vec3f diffuse, vec3f specular, 
		  float constant, float linear, float quadratic, float cosInner = 0.0f, float cosOuter = 0.0f, bool spot = false);
	void setName(const char* n);
	void setPosition(float x, float y, float z);
	void setDirection(float x, float y, float z);
	void setAmbient(float red, float green, float blue);
	void setDiffuse(float red, float green, float blue);
	void setSpecular(float red, float green, float blue);
	void setConstantValue(float kc);
	void setLinearValue(float ki);
	void setQuadraticValue(float kq);
	void setCosInner(float cos);
	void setCosOuter(float cos);
	void setType(bool flag);
	void setOffset(vec3f center, vec3f angle, vec3f radius, vec3f speed);
	const char* getName() const;
	vec3f& getPosition();
	vec3f& getAmbient();
	vec3f& getDiffuse();
	vec3f& getSpecular();
	vec3f& getDirection();
	float getRadius() const;
	float getConstantValue() const;
	float getLinearValue() const;
	float getQuadraticValue() const;
	float getCosInner() const;
	float getCosOuter() const;
	bool getType() const;
	void show();
private:
	const char* name;
	vec3f direction;
	vec3f position;
	vec3f ambient;
	vec3f diffuse;
	vec3f specular;
	float radius;
	float constant;
	float linear;
	float quadratic;
	float cosInner;
	float cosOuter;
	bool type;
};
#endif