#version 460
layout(location = 0) in vec4 position;
out vec2 fragpos;
out vec2 uvcoord;

uniform mat4 projection;

void main(){
	gl_Position = projection * vec4(position.xy, 0.0f, 1.0f);
	fragpos = position.xy;
	uvcoord = position.zw;
}