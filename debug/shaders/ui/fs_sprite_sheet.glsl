#version 460

in vec2 uvcoord;
in vec2 fragpos;
out vec4 fragment;

uniform sampler2D tex;

void main(){
	fragment = texture(tex, uvcoord);
}