#version 460
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
layout(location = 3) in vec3 tangent;


out struct VS_OUT{
	vec3 fragpos;
	vec3 normal;
	vec2 uvcoord;
	vec4 fragPosLightSpace;
	mat3 TBN;
}vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 lightSpaceMatrix;

void main(){
	vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0f)));
	vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0f)));
	T      = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vs_out.TBN   			 = transpose(mat3(T, B, N));
	gl_Position        		 = viewMatrix * modelMatrix * position;
	vs_out.fragpos     		 = vec3(modelMatrix * position);
	vs_out.normal      		 = vec3(transpose(inverse(modelMatrix)) * vec4(normal, 1.0f));
	vs_out.uvcoord     		 = uvcoord;
	vs_out.fragPosLightSpace = lightSpaceMatrix * vec4(vs_out.fragpos, 1.0f);
}