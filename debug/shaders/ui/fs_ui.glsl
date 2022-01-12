#version 460

in vec2 fragpos;
in vec2 uvcoord;
out vec4 fragment;


uniform sampler2D icon;
uniform vec3 blend;

uniform bool clipping;
uniform float borderMax;
uniform float borderMin;

void main(){
	if(clipping){
		if(fragpos.y <= borderMin || fragpos.y >= borderMax){
			discard;
		}
	}
	fragment = texture(icon, uvcoord) * vec4(blend, 1.0f);
}