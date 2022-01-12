#version 460
smooth in vec3 out_color;
in vec2 out_uvcoord;
in vec3 out_normal;
in vec3 out_fragpos;
out vec4 out_fragcolor;

void main(){
	out_fragcolor = vec4(0.5f, 0.0f, 0.5f, 1.0f);
}