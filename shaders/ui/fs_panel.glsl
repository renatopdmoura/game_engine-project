#version 460 core

in vec2 canvasCoord;

out vec4 fragment;

uniform vec4 rgbBorderColor;
uniform vec4 rgbHeaderColor;
uniform vec4 rgbBackgroundColor;
uniform float fOffsetX;
uniform float fOffsetY;
uniform float fPosX;
uniform float fPosY;
uniform float fWidth;
uniform float fHeight;
uniform float fHeaderHeight;
uniform float fBorderThickness;

void main(){
	float fBorderMinX = (fPosX + fOffsetX) + fBorderThickness;
	float fBorderMaxX = (fPosX + fOffsetX) + fWidth - fBorderThickness;
	float fBorderMinY = (fPosY + fOffsetY) + fBorderThickness;
	float fBorderMaxY = (fPosY + fOffsetY) + fHeight - fBorderThickness;

	bool H = canvasCoord.x < fBorderMinX || canvasCoord.x > fBorderMaxX;
	bool V = canvasCoord.y < fBorderMinY || canvasCoord.y > fBorderMaxY;

	float fHeaderMinY = (fPosY + fOffsetY) + fHeight - (fHeaderHeight + fBorderThickness);
	float fHeaderMaxY = (fPosY + fOffsetY) + fHeight;
	
	if(canvasCoord.y > fHeaderMinY && canvasCoord.y < fHeaderMaxY)
		fragment = rgbHeaderColor;
	else
		fragment = rgbBackgroundColor;
	if((H || V) && fBorderThickness > 0.0f)
		fragment = rgbBorderColor;
}