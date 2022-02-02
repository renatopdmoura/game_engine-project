#version 460 core

in vec2 canvasCoord;

out vec4 fragment;

uniform float borderThickness;
uniform vec3 rgbBorderColor;
uniform vec4 rgbColor;
uniform vec3 rgbHeaderColor;

uniform float fOffsetX;
uniform float fOffsetY;
uniform float fPosX;
uniform float fPosY;
uniform float fWidth;
uniform float fHeight;
uniform float fHeaderHeight;

void main(){
	float sw = fWidth / 2;
	float fBorderMinX = (fPosX + fOffsetX) + borderThickness;
	float fBorderMaxX = (fPosX + fOffsetX) + fWidth - borderThickness;
	
	float fBorderMinY = (fPosY + fOffsetY) + borderThickness;
	float fBorderMaxY = (fPosY + fOffsetY) + fHeight - borderThickness;

	bool horizontal = canvasCoord.x < fBorderMinX || canvasCoord.x > fBorderMaxX;
	bool vertical   = canvasCoord.y < fBorderMinY || canvasCoord.y > fBorderMaxY;

	float fHeaderMinY = (fPosY + fOffsetY) + fHeight - (fHeaderHeight + borderThickness);
	float fHeaderMaxY = (fPosY + fOffsetY) + fHeight;
	if(canvasCoord.y > fHeaderMinY && canvasCoord.y < fHeaderMaxY)
		fragment = vec4(rgbHeaderColor, 1.0f);
	else
		fragment = rgbColor;

	if((horizontal ||  vertical) && borderThickness > 0.0f)
		fragment = vec4(rgbBorderColor, 1.0f);

}