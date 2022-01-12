#version 460

in vec2 uvcoord;
in vec2 fragpos;
out vec4 fragment;

uniform sampler2D tex;

void main(){
	fragment = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}