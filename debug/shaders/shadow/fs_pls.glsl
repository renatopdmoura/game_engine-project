#version 460

in vec4 fragpos;

uniform float zFar;
uniform vec3 light_pos;

void main(){
	float lightDistance = length(fragpos.xyz - light_pos);
	lightDistance = lightDistance / zFar;
	gl_FragDepth  = lightDistance;
}