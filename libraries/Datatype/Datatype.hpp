#ifndef DATATYPE_H
#define DATATYPE_H

#include "../Math/Math.hpp"

class PointLight{
public:
	vec4<float> position;
	vec4<float> color;
};

class SpotLight{	
public:
	vec4<float> direction;
	vec4<float> position;
	vec4<float> color;
};

class Sun{
public:
	vec4<float> direction;
	vec4<float> color;
};

class Material{
public:
	vec3<float> albedo;
	float roughness;
	float metallic;
	float ao;
};

#endif