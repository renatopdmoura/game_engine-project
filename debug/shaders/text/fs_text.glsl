#version 460
in vec2 fragpos;
in vec2 uvcoord;
out vec4 fragment;

uniform sampler2D fontTex;
uniform vec3 colorText;

uniform bool clipping;
uniform float borderMin;
uniform float borderMax;

void main(){
	if(clipping){
		if(fragpos.y < borderMin || fragpos.y > borderMax)
			discard;
	}
	vec4 sampled  = vec4(1.0f, 1.0f, 1.0f, texture(fontTex, uvcoord).r);
	fragment      = vec4(colorText, 1.0f) * sampled; 
}