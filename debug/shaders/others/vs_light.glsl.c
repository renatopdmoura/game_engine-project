#version 460
layout(location = 0) in vec4 position; 
layout(location = 1) in vec4 color; 
smooth out vec4 out_color; 

layout(location = 4) uniform mat4 model;
layout(location = 8) uniform mat4 view_projection;

void main(){
	gl_Position  = view_projection * model * position;
	out_color    = color; 
}