#version 460
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uvcoord;

out struct VS_OUT{
	vec2 uvcoord;
	vec3 normal;
	vec3 fragpos;
	mat4 cameraSpace;
	mat4 model;
}vs_out;

layout(binding = 0) uniform objAttrib{
	mat4 offset[3];
}offset_array;

uniform mat4 model;
uniform mat4 viewMatrix;
uniform mat4 cameraSpace;

void main(){
	mat4 transform     = offset_array.offset[gl_InstanceID] * model;
	gl_Position        = viewMatrix * transform * position;
	vs_out.normal      = normalize(vec3(transpose(inverse(cameraSpace * transform)) * normal));
	vs_out.fragpos     = vec3((cameraSpace * transform) * position);
	vs_out.uvcoord     = uvcoord;
	vs_out.cameraSpace = cameraSpace;
	vs_out.model       = transform;
}