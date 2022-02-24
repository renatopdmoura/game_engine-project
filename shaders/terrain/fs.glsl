#version 460 core

// - index size point lights
const int COUNT_POINT_LIGHT = 4;
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
	vec4 position[4];
	vec4 color[4];
}pointLight; 

uniform struct Material{
	sampler2D pathMask[2];
	sampler2D albedo[4];
	sampler2D normal[4];
	sampler2D roughness[4];
	sampler2D ao[4];
	float metallic;
}material;

uniform float fHeight;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main(){
	float fScale = vs_out.fragWorldPos.y / 40.0f;
	vec2 fTexCoords = vs_out.texCoords * 100.0f;
	float fWeight = 0.75f;
	float fTexContribuition = clamp(dot(vs_out.normal, vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f);
	fTexContribuition  = clamp(fTexContribuition - fWeight, 0.0f, 1.0f);
	fTexContribuition /= (1.0f - fWeight);
	float W1 = fTexContribuition;
	float W2 = (1.0f - fTexContribuition);

	vec4 albedo_blended = vec4(0);
	albedo_blended += texture(material.albedo[0], fTexCoords).rgba * W1;
	albedo_blended += texture(material.albedo[1], fTexCoords).rgba * W2;
	
	vec4 normal_blended = vec4(0);
 	normal_blended += texture(material.normal[0], fTexCoords).rgba * W1;
 	normal_blended += texture(material.normal[1], fTexCoords).rgba * W2;

 	vec2 roughness_blended = vec2(0);
	roughness_blended += texture(material.roughness[0], fTexCoords).ra * W1;
	roughness_blended += texture(material.roughness[1], fTexCoords).ra * W2;

	vec2 ao_blended = vec2(0);
	ao_blended += texture(material.ao[0], fTexCoords).ra * W1;
	ao_blended += texture(material.ao[1], fTexCoords).ra * W2;
	
	float mask0		   = texture(material.pathMask[0], vs_out.texCoords).r;	
	vec4 texAlbedo0    = albedo_blended * mask0 + texture(material.albedo[2], fTexCoords) * (1.0f - mask0);
	vec4 texNormal0    = normal_blended * mask0 + texture(material.normal[2], fTexCoords) * (1.0f - mask0);
	vec2 texRoughness0 = roughness_blended * mask0 + texture(material.roughness[2], fTexCoords).ra * (1.0f - mask0);
	vec2 texAO0 	   = ao_blended * mask0 + texture(material.ao[2], fTexCoords).ra * (1.0f - mask0);

	float mask1		   = texture(material.pathMask[1], vs_out.texCoords).r;	
	vec4 texAlbedo1    = texAlbedo0 * mask1 + texture(material.albedo[3], fTexCoords) * (1.0f - mask1);
	vec4 texNormal1    = texNormal0 * mask1 + texture(material.normal[3], fTexCoords) * (1.0f - mask1);
	vec2 texRoughness1 = texRoughness0 * mask1 + texture(material.roughness[3], fTexCoords).ra * (1.0f - mask1);
	vec2 texAO1 	   = texAO0 * mask1 + texture(material.ao[3], fTexCoords).ra * (1.0f - mask1);	

	vec3 albedo     = vec3(texAlbedo1);
	vec3 normal     = vs_out.TBN * vec3(texNormal1);
	float roughness = texRoughness1.r;
	float ao  		= texAO1.r;

	vec3 N = normal;
	vec3 V = normalize(vs_out.cameraPos - vs_out.fragWorldPos);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, material.metallic);

	vec3 Lo = vec3(0.0f);
	for(int i = 0; i < COUNT_POINT_LIGHT; ++i){
		vec3 position  = vs_out.cameraPos;
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
	vec3 ambient = vec3(0.15f) * albedo * ao;
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