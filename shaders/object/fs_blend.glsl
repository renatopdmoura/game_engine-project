#version 460 core

out vec4 fragment;

in struct VS_OUT{
	vec3 fragWorldPos;
	vec3 cameraPos;
	vec2 texCoords;
	mat3 TBN;
	vec3 normal;
}vs_out;

uniform sampler2D tex;

void main(){
	fragment = texture(tex, vs_out.texCoords);
}