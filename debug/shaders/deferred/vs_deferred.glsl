#version 460 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
layout(location = 3) in vec3 tangent;

out struct VS_OUT{
	vec3 fragpos;
	// vec3 normal;
	vec2 uvcoord;
	mat3 TBN;
}vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main(){
	vec3 norm  = vec3(transpose(inverse(modelMatrix)) * vec4(normal, 1.0f));
	vec3 T 	   = normalize(vec3(modelMatrix * vec4(tangent, 0.0f)));
	vec3 N     = normalize(vec3(modelMatrix * vec4(norm, 0.0f)));
	T          = normalize(T - dot(T, N) * N);
	vec3 B 	   = cross(N, T);
	vs_out.TBN = mat3(T, B, N);
	// vs_out.normal  = normal;
	vs_out.fragpos = vec3(modelMatrix * position);
	vs_out.uvcoord = uvcoord;
	gl_Position    = viewMatrix * modelMatrix * position;
}