#version 460
layout(location = 0) in vec2 position;
out vec2 fragpos;

uniform mat4 uiMatrix;

void main(){
	fragpos     = position; 
	gl_Position = uiMatrix * vec4(position, 0.0f, 1.0f);
}