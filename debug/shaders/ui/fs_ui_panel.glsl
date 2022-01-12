#version 460

in vec2 fragpos;
in vec2 uvcoord;
out vec4 fragment;

uniform float w;
uniform float h;
uniform float x; 
uniform float y;

uniform float thickness;
uniform vec3 fill;
uniform vec3 border;
uniform sampler2D rampTex;

uniform bool clipping;
uniform float borderMin;
uniform float borderMax;
uniform float style;
uniform float opacity;

void main(){
	if(clipping){
		if(fragpos.y <= borderMin || fragpos.y >= borderMax){
			discard;
		}
	}
	if(thickness > 0 && style > -1){
		if(style == 0){
			bool axisx = fragpos.x > (x + thickness) && fragpos.x < (x + w - thickness);
			bool axisy = fragpos.y > (y + thickness) && fragpos.y < (y + h - thickness);
			if(axisx && axisy){
				fragment = vec4(fill, opacity);
			}
			else{
				float lerp = texture(rampTex, uvcoord).r;
				fragment   = vec4(border * lerp,  1.0f);
			}
		}
		else if(style == 1){
			bool axisx = fragpos.x > (x + thickness) && fragpos.x < (x + w - thickness);
			if(axisx){
				fragment = vec4(fill, opacity);
			}
			else{
				float lerp = texture(rampTex, uvcoord).r;
				fragment   = vec4(border * lerp,  1.0f);
			}
		}
		else if(style == 2){
			bool axisx = fragpos.x > (x + thickness);
			if(axisx){
				fragment = vec4(fill, opacity);
			}
			else{
				float lerp = texture(rampTex, uvcoord).r;
				fragment   = vec4(border * lerp,  1.0f);
			}
		}
		else if(style == 3){
			bool axisx = fragpos.x < (x + w - thickness);
			if(axisx){
				fragment = vec4(fill, opacity);
			}
			else{
				float lerp = texture(rampTex, uvcoord).r;
				fragment   = vec4(border * lerp,  1.0f);
			}
		}
		else if(style == 4){
			bool axisy = fragpos.y < (y + h - thickness);
			if(axisy){
				fragment = vec4(fill, opacity);
			}
			else{
				float lerp = texture(rampTex, uvcoord).r;
				fragment   = vec4(border * lerp,  1.0f);
			}
		}
		else if(style == 5){
			bool axisy = fragpos.y > (y + thickness);
			if(axisy){
				fragment = vec4(fill, opacity);
			}
			else{
				float lerp = texture(rampTex, uvcoord).r;
				fragment   = vec4(border * lerp,  1.0f);
			}
		}
	}
	else{
		fragment = vec4(fill, opacity);
	}	
}