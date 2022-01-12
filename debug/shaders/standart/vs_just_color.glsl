#version 460 

layout(location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main(){
	gl_Position = viewMatrix * modelMatrix * position;
}
