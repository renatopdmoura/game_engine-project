#version 460
layout(location = 0) in vec4 position; 
layout(location = 1) in vec2 uvcoord;
out vec2 out_uvcoord;

void main(){
	gl_Position  = position;
	out_uvcoord  = uvcoord;
}