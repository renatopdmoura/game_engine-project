#version 460 core

out vec4 fragment;

uniform float zNear;
uniform float zFar;

void main(){
	// - gl_FragCoord.z range is [0.0f, 1.0f] by projection operations. Need to ndc[-1.0f, 1.0f]
	float ndc    = (gl_FragCoord.z * 2.0f - 1.0f);
	// - Remap non linear value to linear value (more shininess how much greteast is distance)
	float result = (2.0f * zNear * zFar) / (zFar + zNear - ndc * (zFar - zNear));
	// - At non linear value, z coordinate is near to 1.0f. Divide by far to convert z to [0.0f, 1.0] range again
	// and values will be approximate to 0.0f how much z minor is.
	fragment     = vec4(vec3(result / zFar), 1.0f);
}