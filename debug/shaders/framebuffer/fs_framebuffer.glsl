#version 460
in vec2 uvcoord;
out vec4 fragment;

uniform sampler2D tex;

const float offset = 1.0f / 300.0f;
float LinearizeDepth(float depth){
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * 0.1f * 7.5f) / (7.5f + 0.1f - z * (7.5f - 0.1f));
}

void main(){
	// float depthValue = texture(tex, uvcoord).r;
	// fragment = vec4(vec3(depthValue), 1.0f);
	// out_fragcolor = vec4(vec3(LinearizeDepth(depthValue) / 7.5f), 1.0f);

	// out_fragcolor = texture(tex, out_uvcoord);

	//Inverse
	// out_fragcolor = vec4(vec3(1.0f - texture(tex, out_uvcoord)), 1.0f);

	//Gray scale
	// vec4 fragColor = texture(tex, out_uvcoord);
	// float average = (0.2126f * fragColor.r + 0.7152f * fragColor.g + 0.0722 * fragColor.b);
	// out_fragcolor = vec4(average, average, average, 1.0f);

	// Kernel effects
	// vec2 offsets[9] = vec2[](
	// 	vec2(-offset, offset),
	// 	vec2(0.0f,    offset),
	// 	vec2(offset,  offset),
	// 	vec2(-offset, 0.0f),
	// 	vec2(0.0f,    0.0f),
	// 	vec2(offset,  0.0f),
	// 	vec2(-offset, -offset),
	// 	vec2(0.0f,    -offset),
	// 	vec2(offset, -offset)
	// );
	// float kernelSharpen[9] = float[](
	// 	-1, -1, -1,
	// 	-1,  9, -1,
	// 	-1, -1, -1
	// );
	// float kernelBlur[9] = float[](
	// 	1.0f/16, 2.0f/16, 1.0f/16,
	// 	2.0f/16, 4.0f/16, 2.0f/16,
	// 	1.0f/16, 2.0f/16, 1.0f/16
	// );
	// float kernelEdgeDetection[9] = float[](
	// 	1, 1, 1,
	// 	1, -8, 1,
	// 	1, 1, 1
	// );
	// vec3 sampleTex[9];
	// for(int i = 0; i < 9; ++i){
	// 	sampleTex[i] = vec3(texture(tex, vs_out.uvcoord.st + offsets[i]));
	// }
	// vec3 col = vec3(0.0f);
	// for(int i = 0; i < 9; ++i){
	// 	col += sampleTex[i] * kernelSharpen[i];
	// }
	// out_fragcolor = vec4(col, 1.0f);
}