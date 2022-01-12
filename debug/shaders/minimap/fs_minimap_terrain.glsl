#version 460

in struct VS_OUT{
	vec3 fragpos;
	vec3 normal;
	vec2 uvcoord;
}vs_out;

out vec4 fragment;

uniform struct Material{
	//Grass
	sampler2D texLayer0Diff;
	sampler2D texLayer0Spec;
	//Rock
	sampler2D texLayer1Diff;
	sampler2D texLayer1Spec;
	//Path
	sampler2D texLayerPath0Map;
	sampler2D texLayerPath0Diff;
	sampler2D texLayerPath0Spec;
	sampler2D texLayerPath1Map;
	sampler2D texLayerPath1Diff;
	sampler2D texLayerPath1Spec;
}material;

uniform float fWeight;
uniform float fMaxHeight;
uniform float fTexcoord;

vec4 resultDiff = vec4(0);

void slopeBasedLayers(){
	//Contribuição com base na normal da superfície
	float fTexContribuition = clamp(dot(vs_out.normal, vec3(0.0f, 1.0f, 0.0f) - 0.1f), 0.0f, 1.0f);
	float fNormal = fTexContribuition;
	//Altura máxima no intervalo [0, 1]
	float fScale = vs_out.fragpos.y / fMaxHeight;
	//Interpolação do fator de contribuição
	fTexContribuition = clamp(fTexContribuition - fWeight, 0.0f, 1.0f);
	fTexContribuition /= (1.0f - fWeight);
	//Quanto maior o valor de fTexContribuition, o peso W1 contribui mais e o inverso também é verdade
	float W1 = fTexContribuition;
	//Quanto maior o valor de fTexContribuition, o peso W2 contribui menos e o inverso também é verdade
	float W2 = (1.0f - fTexContribuition);
	//Escalonamento UV
	vec2 fuvcoord = vs_out.uvcoord * fTexcoord;

	vec4 sampledDiff = vec4(0);
	
	sampledDiff += texture(material.texLayer0Diff, fuvcoord) * W1; 
	sampledDiff += texture(material.texLayer1Diff, fuvcoord) * W2;
	
	float mapLerp0 = texture(material.texLayerPath0Map, vs_out.uvcoord).r;
	vec4 sampledPath0Diff = texture(material.texLayerPath0Diff, fuvcoord);

	float mapLerp1 = texture(material.texLayerPath1Map, vs_out.uvcoord).r;
	vec4 sampledPath1Diff = texture(material.texLayerPath1Diff, fuvcoord);

	resultDiff = (sampledDiff * mapLerp0 + sampledPath0Diff * (1.0f - mapLerp0)) * mapLerp1 + sampledPath1Diff * (1.0f - mapLerp1);
}

void main(){
	slopeBasedLayers();
	float gamma = 2.2f;
	fragment = vec4(pow(resultDiff.rgb, vec3(1.0f / gamma)), 1.0f);
	// fragment = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}