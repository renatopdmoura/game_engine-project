#version 460

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out struct VS_OUT{
	vec3 normal;
	vec3 fragpos;
	vec3 lightPos;
	vec3 direction;
}vs_out;

uniform mat4 model;
uniform mat4 viewMatrix;
uniform mat4 cameraSpace;
// uniform mat4 normalMatrix;

void main(){
	gl_Position      = viewMatrix * model * position;
	vs_out.normal    = normalize(vec3(transpose(inverse(cameraSpace * model)) * normal));
	vs_out.fragpos   = vec3((cameraSpace * model) * position);
	vs_out.lightPos  = vec3((cameraSpace * model) * vec4(0.0f, 2.0f, 0.0f, 1.0f));
	vs_out.direction = vec3(cameraSpace * vec4(0.0f, -1.0f, 0.0f, 0.0f));
}