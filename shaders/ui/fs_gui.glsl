#version 460 core

in vec2 canvasCoord;

out vec4 fragment;

uniform vec3 ID;

uniform float fOffsetX;
uniform float fOffsetY;
uniform float fPosY;
uniform float fHeight;
uniform float fHeaderHeight;

void main(){
	float fHeaderMinY = (fPosY + fOffsetY) + fHeight - fHeaderHeight;
	float fHeaderMaxY = (fPosY + fOffsetY) + fHeight;
	if(canvasCoord.y > fHeaderMinY && canvasCoord.y < fHeaderMaxY)
		fragment = vec4(ID, 1.0f);
	else
		fragment = vec4(vec3(0.5f, 0.0f, 0.5f), 0.8f);
}