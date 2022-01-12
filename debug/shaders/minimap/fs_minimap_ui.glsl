#version 460
in vec2 uvcoord;
out vec4 fragment;

uniform sampler2D minimap;
uniform sampler2D minimapMask;
uniform sampler2D minimapEdge;

void main(){
	float value = texture(minimapMask, uvcoord).r;
	vec4 result = texture(minimapEdge, uvcoord) * value + (texture(minimap, uvcoord) * (1.0f - value));
	fragment    = result;

}