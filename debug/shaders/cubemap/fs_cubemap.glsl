#version 460
in vec3 texcoord;
out vec4 fragment;

uniform samplerCube skybox;
uniform float opacity;

void main(){
	fragment = texture(skybox, texcoord) * vec4(1.0f, 1.0f, 1.0f, opacity);
}