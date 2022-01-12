#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;

in struct VS_OUT{
	vec3 fragpos;
	// vec3 normal;
	vec2 uvcoord;
	mat3 TBN;
}vs_out;

uniform struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
}material;

void main(){
	//vertices at the world space + shininess intensity
	gPosition.rgb  = vs_out.fragpos;
	gPosition.a    = material.shininess;
	
	//to use normal map, transform normals from tangent space to world space
	vec3 normal  = texture(material.normal, vs_out.uvcoord).rgb;
	normal 		 = normal * 2.0f - 1.0f;
	gNormal      = normalize(vs_out.TBN * normal);
	// gNormal       = vs_out.normal;
	
	//get diffuse color and specular strength
	gAlbedo.rgb  = texture(material.diffuse, vs_out.uvcoord).rgb;
	gAlbedo.a    = texture(material.specular, vs_out.uvcoord).r;
}