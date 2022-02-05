#version 460 core

in vec2 texCoords;
out vec4 fragment;

uniform sampler2D tex;
uniform vec3 color;

void main(){
	vec4 result   = vec4(vec3(1.0f), texture(tex, texCoords).r);
	fragment.rgba = vec4(color, 1.0f) * result;
	// fragment.rgba = texture(tex, texCoords);
}