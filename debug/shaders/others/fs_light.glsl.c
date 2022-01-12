#version 460
out vec4 out_fragcolor; 
smooth in vec4 out_color;

uniform vec3 colorLight; 

void main(){
	out_fragcolor = vec4(colorLight, 1.0f);
}