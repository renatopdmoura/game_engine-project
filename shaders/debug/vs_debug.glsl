#version 460 core

layout(location = 0) in vec4 position;

uniform mat4 viewProj;
uniform mat4 model;

void main(){
	gl_Position = viewProj * model * position;
}