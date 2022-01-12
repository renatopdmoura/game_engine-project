#version 460 core

in vec2 uvcoord;
out vec4 fragment;

uniform sampler2D tex;

void main(){
	float depthValue = texture(tex, uvcoord).r;
	fragment = vec4(vec3(depthValue), 1.0f);
}