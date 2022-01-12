#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;

out struct VS_OUT{
	vec3 fragWorldPos;
	vec2 texCoords;
	vec3 normal;
	mat3 TBN;
}vs_out;

uniform mat4 viewProj;
uniform mat4 model;
uniform mat4 normalSpace;

void main(){
	// mat3 NS = mat3(normalSpace);
	mat3 NS = mat3(transpose(inverse(model)));
	vec3 T  = normalize(NS * tangent);
	vec3 N  = normalize(NS * normal);
	T       = normalize(T - dot(T, N) * N);
	vec3 B  = NS * cross(N, T);
	vs_out.TBN          = mat3(T, B, N);
	vs_out.normal 		= NS * normal;
	vs_out.fragWorldPos = vec3(model * position);
	vs_out.texCoords    = texCoords;
	gl_Position         = viewProj * model * position;
}