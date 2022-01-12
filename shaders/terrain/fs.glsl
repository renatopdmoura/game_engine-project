#version 460 core

// - index size point lights
const int COUNT_POINT_LIGHT = 4;
const float PI = 3.14159265359;

in struct VS_OUT{
	vec3 fragWorldPos;
	vec2 texCoords;
	vec3 normal;
	mat3 TBN;
}vs_out;

out vec4 fragment;

layout(binding = 0, shared) uniform PointLight{
	vec4 position[4];
	vec4 color[4];
}pointLight; 

uniform struct Material{
	sampler2D albedo;
	sampler2D normal;
	sampler2D roughness;
	sampler2D ao;
	float metallic;
}material;

uniform vec3 cameraPos;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main(){
	vec3 albedo     = texture(material.albedo, vs_out.texCoords * 100.0f).rgb;
	vec3 normal     = normalize(vs_out.TBN * texture(material.normal, vs_out.texCoords * 100.0f).rgb);
	float roughness = texture(material.roughness, vs_out.texCoords * 100.0f).r;
	float ao        = texture(material.ao, vs_out.texCoords * 100.0f).r;
	
	vec3 N = vs_out.normal;
	vec3 V = normalize(cameraPos - vs_out.fragWorldPos);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, material.metallic);

	vec3 Lo = vec3(0.0f);
	for(int i = 0; i < COUNT_POINT_LIGHT; ++i){
		vec3 position = vec3(pointLight.position[i]);
		vec3 colors    = vec3(pointLight.color[i]);
		//Calculate per-light radiance
		vec3 L = normalize(position - vs_out.fragWorldPos);
		vec3 H = normalize(L + V);
		float distance    = length(position - vs_out.fragWorldPos);
		float attenuation = 1.0f / (distance * distance);
		vec3 radiance 	  = colors * attenuation;

		//Cook-Torrance BRDF
		float NDF = distributionGGX(N, H, roughness);
		float G   = geometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlick(max(dot(H, V), 0.1f), F0);
		vec3 kS   = F;
		vec3 kD   = vec3(1.0f) - kS;
		kD *= 1.0f - material.metallic;

		vec3 numerator    = NDF * G * F;
		float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
		vec3 specular     = numerator / denominator;
			
		float NdotL = max(dot(N, L), 0.0f);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	vec3 ambient = vec3(0.05f) * albedo * ao;
	vec3 color   = ambient + Lo;
	color        = color / (color + vec3(1.0f));
	color        = pow(color, vec3(1.0f / 2.2f));
	fragment     = vec4(color, 1.0f);
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
