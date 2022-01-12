#version 460

in vec2 fragpos;
in vec2 uvcoord;
out vec4 fragment;

uniform sampler2D rampTex;
uniform float thickness;
uniform vec3 fill;
uniform vec3 border;
uniform vec3 blend;

uniform float w;
uniform float h;
uniform float x; 
uniform float y;

uniform bool clipping;
uniform float borderMin;
uniform float borderMax;

void main(){
	if(clipping){
		if(fragpos.y < borderMin || fragpos.y > borderMax){
			discard;
		}
	}
	bool axisx = fragpos.x > (x + thickness) && fragpos.x < (x + w - thickness);
	bool axisy = fragpos.y > (y + thickness) && fragpos.y < (y + h - thickness);
	if(axisx && axisy){
		fragment = vec4(fill * blend, 1.0f);
	}
	else{
		float lerp = texture(rampTex, uvcoord).r;
		fragment   = vec4(border * lerp,  1.0f);
	}
}