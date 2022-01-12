#version 460 core

out vec4 fragment;

uniform vec3 albedo;

void main(){
	fragment = vec4(albedo, 1.0f);
}