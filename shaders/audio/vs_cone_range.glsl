#version 460

layout(location = 0) in vec4 position;

layout(binding = 3, shared) uniform Var{
	mat4 viewProj;
	vec4 cameraPos;
};

uniform mat4 model;

void main(){
	gl_Position  = viewProj * (model * position);
}