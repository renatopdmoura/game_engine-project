#version 460 core 

in vec2 uvcoord;
out vec4 fragment;

uniform sampler2D tex;

void main(){
	fragment = texture(tex, uvcoord);
}