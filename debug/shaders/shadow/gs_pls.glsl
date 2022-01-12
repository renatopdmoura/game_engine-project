#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;
out vec4 fragpos;

uniform mat4 shadowMatrices[6];

void main(){
	for(int face = 0; face < 6; ++face){
		gl_Layer = face;
		for(int i = 0; i < 3; ++i){
			fragpos     = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * fragpos;
			EmitVertex();
		}
		EndPrimitive();
	}
}
