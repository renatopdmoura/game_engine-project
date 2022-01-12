#version 460

in struct VS_OUT{
	vec3 fragpos;
	vec3 normal;
	vec2 uvcoord;
}vs_out;

out vec4 fragment;

uniform sampler2D diffuse;

void main(){
	vec3 result   = vec3(texture(diffuse, vs_out.uvcoord)); 
	float gamma   = 2.2f;
	fragment.rgba = vec4(pow(result.rgb, vec3(1.0f / gamma)), 1.0f);
}