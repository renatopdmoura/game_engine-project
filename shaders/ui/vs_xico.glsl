#version 460 core

layout(location = 0) in vec4 position;

layout(binding = 5, shared) uniform GUI{
	mat4 canvasToClip;
};

void main(){
	gl_Position = canvasToClip * vec4(position.xy, 0.0f, 1.0f);
}