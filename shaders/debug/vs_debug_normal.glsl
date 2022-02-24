#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out VS_OUT{
	vec3 normal;
}vs_out;

uniform mat4 view;
uniform mat4 debug_normal_model;

void main(){
	mat3 NS       = mat3(transpose(inverse(view * debug_normal_model)));
	vs_out.normal = normalize(NS * normal);
	gl_Position   = view * debug_normal_model * position;
}
