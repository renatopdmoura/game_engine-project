#version 460

layout(location = 0) in vec4 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main(){
	gl_Position = lightSpaceMatrix * modelMatrix * position;
}
