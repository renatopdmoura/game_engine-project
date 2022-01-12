#version 460
#define NR_POINT_LIGHTS 1

in struct VS_OUT{
	vec3 fragpos;
	vec3 normal;
	vec2 uvcoord;
	vec4 fragPosLightSpace;
	mat3 TBN;
}vs_out;

out vec4 fragment;

uniform struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
}directionalLight;

uniform struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
}pointLight[NR_POINT_LIGHTS];

uniform struct Material{
	//Grass
	sampler2D texLayer0Diff;
	sampler2D texLayer0Spec;
	sampler2D texLayer0Norm;
	//Rock
	sampler2D texLayer1Diff;
	sampler2D texLayer1Spec;
	sampler2D texLayer1Norm;
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
vec4 resultSpec = vec4(0);
vec4 resultNorm = vec4(0);

uniform vec3 camerapos;
uniform sampler2D depthMap;

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
	vec4 sampledSpec = vec4(0);
	vec4 sampledNorm = vec4(0);

	
	sampledDiff += texture(material.texLayer0Diff, fuvcoord) * W1; 
	sampledSpec += texture(material.texLayer0Spec, fuvcoord) * W1; 
	sampledNorm += texture(material.texLayer0Norm, fuvcoord) * W1; 

	sampledDiff += texture(material.texLayer1Diff, fuvcoord) * W2;
	sampledSpec += texture(material.texLayer1Spec, fuvcoord) * W2;
	sampledNorm += texture(material.texLayer1Norm, fuvcoord) * W1; 
	
	float mapLerp0 = texture(material.texLayerPath0Map, vs_out.uvcoord).r;
	vec4 sampledPath0Diff = texture(material.texLayerPath0Diff, fuvcoord);
	vec4 sampledPath0Spec = texture(material.texLayerPath0Spec, fuvcoord);

	float mapLerp1 = texture(material.texLayerPath1Map, vs_out.uvcoord).r;
	vec4 sampledPath1Diff = texture(material.texLayerPath1Diff, fuvcoord);
	vec4 sampledPath1Spec = texture(material.texLayerPath1Spec, fuvcoord);

	resultDiff = (sampledDiff * mapLerp0 + sampledPath0Diff * (1.0f - mapLerp0)) * mapLerp1 + sampledPath1Diff * (1.0f - mapLerp1);
	resultSpec = (sampledSpec * mapLerp0 + sampledPath0Spec * (1.0f - mapLerp0)) * mapLerp1 + sampledPath1Spec * (1.0f - mapLerp1);
	// resultNorm = (texture(material.texLayer0Norm, fuvcoord) * mapLerp0 + texture(material.texLayer1Norm, fuvcoord) * (1.0f - mapLerp0));
}

float computeShadow(vec4 fragPosLightSpace){
	vec3 projCoords    = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5f + 0.5f;
	float closesDepth  = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias 		   = max(0.5f * (1.0f - dot(vs_out.normal, normalize(-directionalLight.direction))), 0.005f);
	float shadow 	   = 0.0f;
	vec2 texelSize	   = 1.0f / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth? 1.0f: 0.0f;
		}
	}
	shadow /= 9.0f;
	if(projCoords.z > 1.0f) 
		shadow = 0.0f;
	return shadow;
}

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	//Ambient
	vec3 ambient  = light.ambient * vec3(resultDiff);
	
	//Diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff    = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse  = light.diffuse * diff * vec3(resultDiff);

	//Specular

	// Phong
	// vec3 reflectDir = reflect(-lightDir, normal);
	// float spec  	= pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);

	//Blinn-Phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec      = pow(max(dot(halfwayDir, normal), 0.0f), 32.0f * 4.0f);
	
	vec3 specular   = light.specular * spec * vec3(resultSpec);
	float shadow 	= computeShadow(vs_out.fragPosLightSpace);
	return ((ambient + (1.0f - shadow)) * (diffuse + specular));
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 viewDir){
	//Ambient
	vec3 ambient  = light.ambient * vec3(resultDiff);

	//Diffuse
	vec3 lightDir = normalize(light.position - vs_out.fragpos);
	float diff    = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse  = light.diffuse * diff * vec3(resultDiff);

	//Specular
	
	//Phong
	// vec3 reflectDir = reflect(-lightDir, vs_out.normal);
	// float spec 		= pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	
	//Blinn-Phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec      = pow(max(dot(halfwayDir, normal), 0.0f), 32.0f * 4.0f);
	
	vec3 specular   = light.specular * spec * vec3(resultSpec);

	//Atenuation
	float distance    = length(light.position - vs_out.fragpos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main(){
	slopeBasedLayers();
	vec3 normal  = normalize(vs_out.normal);
	vec3 viewDir = normalize(camerapos - vs_out.fragpos);
	vec3 result  = computeDirectionalLight(directionalLight, normal, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; ++i){
		result += computePointLight(pointLight[i], normal, viewDir);
	}

	//Gama correction method 2
	float gamma = 2.2f;
	fragment.rgba = vec4(pow(result.rgb, vec3(1.0f / gamma)), 1.0f);
}