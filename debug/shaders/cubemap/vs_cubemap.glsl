#version 460
layout(location = 0) in vec3 position;
out vec3 texcoord;

uniform mat4 view;

void main(){
	texcoord = position;
	vec4 pos = view * vec4(position, 1.0f);
	gl_Position = pos.xyww;
}