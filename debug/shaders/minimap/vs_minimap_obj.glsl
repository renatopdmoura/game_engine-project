#version 460

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uvcoord;

out struct VS_OUT{
	vec3 fragpos;
	vec3 normal;
	vec2 uvcoord;
}vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main(){
	gl_Position    = viewMatrix * modelMatrix * position;
	vs_out.fragpos = vec3(modelMatrix * position);
	vs_out.normal  = vec3(transpose(inverse(modelMatrix)) * normal);
	vs_out.uvcoord = uvcoord;
}