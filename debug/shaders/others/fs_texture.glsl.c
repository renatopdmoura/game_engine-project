#version 460
smooth in vec3 out_color; 
in vec2 out_uvcoord;
out vec4 out_fragcolor; 

uniform sampler2D texture01;
uniform sampler2D texture02;

void main(){
	out_fragcolor = mix(texture(texture02, out_uvcoord), texture(texture01, out_uvcoord), 0.2f) * vec4(out_color, 1.0f);
}