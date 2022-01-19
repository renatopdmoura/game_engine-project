#version 460 core

layout(location = 0) in vec4 position;

uniform mat4 model;
uniform mat4 viewProjection;

void main(){
	gl_Position = viewProjection * (model * position);
}