#version 460

in vec2 uvcoord;
out vec4 fragment;

uniform vec3 color;
uniform sampler2D arrow;

void main(){
	fragment = vec4(color, 1.0f) *  texture(arrow, uvcoord);
}