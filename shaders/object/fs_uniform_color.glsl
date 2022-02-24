#version 460 core

// - index size point lights
const int COUNT_POINT_LIGHT = 3;
const int COUNT_SPOT_LIGHT = 1;
const float PI = 3.14159265359;

in struct VS_OUT{
	vec3 fragWorldPos;
	vec3 cameraPos;
	vec2 texCoords;
	mat3 TBN;
	vec3 normal;
}vs_out;

out vec4 fragment;

layout(binding = 0, shared) uniform PointLight{
	vec4 position[3];
	vec4 color[3];
}pointLight; 

layout(binding = 1, shared) uniform SpotLight{
	vec4 direction[1];
	vec4 position[1];
	vec4 color[1];
}spotLight;

layout(binding = 2, shared) uniform Sun{
	vec4 direction;
	vec4 color;
}sun;

uniform struct Material{
	vec3 albedo;
	float roughness;
	float ao;
	float metallic;
}material;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 computePointLight(vec3 position, vec3 color, vec3 normal, vec3 viewDir, vec3 lightDir, vec3 F0);
vec3 computeSpotLight(vec3 direction, vec3 position, float cosOuter, float cosInner, vec3 color, vec3 normal, vec3 viewDir, vec3 lightDir, vec3 F0);

void main(){
	vec3 N = vs_out.normal;
	vec3 V = normalize(vs_out.cameraPos - vs_out.fragWorldPos);
	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, material.albedo, material.metallic);
	vec3 Lo = vec3(0.0f);
	for(int i = 0; i < COUNT_SPOT_LIGHT; ++i){
		vec3 L  	   = normalize(vec3(spotLight.position[i]) - vs_out.fragWorldPos);
		float cosOuter = spotLight.position[i].w;
		float cosInner = spotLight.direction[i].w;
		Lo += computeSpotLight(vec3(spotLight.direction[i]), vec3(spotLight.position[i]), cosOuter, cosInner, vec3(spotLight.color[i]), N, V, L, F0);
	}
	for(int i = 0; i < COUNT_POINT_LIGHT; ++i)
		Lo += computePointLight(vec3(pointLight.position[i]), vec3(pointLight.color[i]), N, V, normalize(vec3(pointLight.position[i]) - vs_out.fragWorldPos), F0);
	vec3 ambient = vec3(sun.color) * material.albedo * material.ao;
	vec3 color   = ambient + Lo;
	color        = color / (color + vec3(1.0f));
	color        = pow(color, vec3(1.0f / 2.2f));
	fragment = vec4(color, 1.0f);
}

vec3 computePointLight(vec3 position, vec3 color, vec3 normal, vec3 viewDir, vec3 lightDir, vec3 F0){
	vec3 halfwayDir   = normalize(lightDir + viewDir);
	float distance    = length(position - vs_out.fragWorldPos);
	float attenuation = 1.0f / (distance * distance);
	vec3 radiance 	  = color * attenuation;

	//Cook-Torrance BRDF
	float NDF = distributionGGX(normal, halfwayDir, material.roughness);
	float G   = geometrySmith(normal, viewDir, lightDir, material.roughness);
	vec3 F    = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.1f), F0);
	vec3 kS   = F;
	vec3 kD   = vec3(1.0f) - kS;
	kD *= 1.0f - material.metallic;

	vec3 numerator    = NDF * G * F;
	float denominator = 4.0f * max(dot(normal, viewDir), 0.0f) * max(dot(normal, lightDir), 0.0f) + 0.0001f;
	vec3 specular     = numerator / denominator;
				
	float NdotL = max(dot(normal, lightDir), 0.0f);
	return (kD * material.albedo / PI + specular) * radiance * NdotL;
}

vec3 computeSpotLight(vec3 direction, vec3 position, float cosOuter, float cosInner, vec3 color, vec3 normal, vec3 viewDir, vec3 lightDir, vec3 F0){
	float theta = dot(lightDir, normalize(direction));
	if(theta > cosOuter){
		float intensity = clamp((theta - cosOuter) / (cosInner - cosOuter), 0.0f, 1.0f);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float distance    = length(position - vs_out.fragWorldPos);
		float attenuation = 1.0f / (distance * distance);
		vec3 radiance 	  = color * attenuation;

		//Cook-Torrance BRDF
		float NDF = distributionGGX(normal, halfwayDir, material.roughness);
		float G   = geometrySmith(normal, viewDir, lightDir, material.roughness);
		vec3 F    = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.1f), F0);
		vec3 kS   = F;
		vec3 kD   = vec3(1.0f) - kS;
		kD *= 1.0f - material.metallic;

		vec3 numerator    = NDF * G * F;
		float denominator = 4.0f * max(dot(normal, viewDir), 0.0f) * max(dot(normal, lightDir), 0.0f) + 0.0001f;
		vec3 specular     = numerator / denominator;
				
		float NdotL = max(dot(normal, lightDir), 0.0f);
		return (kD * material.albedo / PI + specular) * radiance * NdotL * intensity;
	}
	else
		return vec3(0.0f);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

float distributionGGX(vec3 N, vec3 H, float roughness){
	float a  	 = roughness * roughness;
	float a2 	 = a * a;
	float NdotH  = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;
	float num    = a2;
	float denom  = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom        = PI * denom * denom;
	return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness){
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;
	float num   = NdotV;
	float denom = NdotV * (1.0f - k) + k;
	return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}
