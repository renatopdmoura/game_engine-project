#version 460
layout(location = 0) in vec4 position; 
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uvcoord;
out vec2 out_uvcoord;
out vec3 out_normal;
out vec3 out_fragpos;

layout(location = 4) uniform mat4 model;
layout(location = 8) uniform mat4 projection;

void main(){
	gl_Position  = projection * ((position * vec4(0.15f, 0.15f, 0.45f, 1.0f)) + vec4(0.75f, -0.75f, -1.15f, 0.0f)); 
	out_uvcoord  = uvcoord;
	out_fragpos  = vec3(model * position);
	out_normal   = vec3(transpose(inverse(model)) * normal);
}