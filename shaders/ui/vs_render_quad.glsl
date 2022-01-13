#version 460 core

layout(location = 0) in vec4 position;
out vec2 texCoords;

uniform mat4 projection;

void main(){
	texCoords   = position.zw;
	gl_Position = projection * vec4(position.xy, 0.0f, 1.0f);
}
