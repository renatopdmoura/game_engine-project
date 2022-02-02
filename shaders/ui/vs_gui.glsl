#version 460 core

layout(location = 0) in vec4 position;

out vec2 canvasCoord;
out vec2 texCoords;

layout(binding = 5, shared) uniform GUI{
	mat4 canvasToClip;
};

uniform float fOffsetX;
uniform float fOffsetY;

void main(){
	float x = position.x + fOffsetX;
	float y = position.y + fOffsetY;
	canvasCoord = vec2(x, y);
	texCoords   = position.zw;
	gl_Position = canvasToClip * vec4(x, y, 0.0f, 1.0f);
}