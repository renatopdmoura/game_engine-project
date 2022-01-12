#version 460

layout(location = 0) in vec4 position;
out vec2 uvcoord;

uniform mat2 rotate;

void main(){
	gl_Position = vec4(rotate * position.xy, 0.0f, 10.0f);
	uvcoord 	= position.zw;

}