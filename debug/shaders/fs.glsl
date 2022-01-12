#version 460
in vec2 fragpos;
out vec4 fragment;

uniform float x;
uniform float y;
uniform float w;
uniform float h;
uniform vec3 fillColor;
uniform vec3 borderColor;
uniform float blend;
uniform float thickness;

void main(){
	bool axisx = fragpos.x > (x + thickness) && fragpos.x < (x + w - thickness);
	bool axisy = fragpos.y > (y + thickness) && fragpos.y < (y + h - thickness);
	if(axisx && axisy)
		fragment = vec4(fillColor * blend, 1.0f);
	else
		fragment = vec4(borderColor, 1.0f);
}