#version 460 core

in vec2 texCoords;
out vec4 fragment;

uniform sampler2D tex;

void main(){
	float gamma   = 2.2f;
	fragment.rgba = vec4(pow(texture(tex, texCoords).rgb, vec3(1.0f / gamma)), 1.0f);
}