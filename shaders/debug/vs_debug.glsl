#version 460 core

layout(location = 0) in vec4 position;

layout(binding = 3, shared) uniform Var{
	mat4 viewProj;
	vec4 cameraPos;
};

uniform mat4 debug_depth_model;

void main(){
	gl_Position = viewProj * debug_depth_model * position;
}