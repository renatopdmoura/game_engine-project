#version 460
#define NR_POINT_LIGHTS 4

in struct VS_OUT{
	vec2 uvcoord;
	vec3 normal;
	vec3 fragpos;
	vec4 fragpos_light_space;
}vs_out;

out vec4 out_fragcolor;

uniform struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
}material;

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

uniform vec3 camerapos;
uniform sampler2D depthMap;

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDir);
float computeShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main(){
	vec3 normal  = normalize(vs_out.normal);
	vec3 viewDir = normalize(camerapos - vs_out.fragpos);
	vec3 result  = computeDirectionalLight(directionalLight, normal, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += computePointLight(pointLight[i], normal, vs_out.fragpos, viewDir);
	}
	out_fragcolor = vec4(result, 1.0f);
}

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir   = normalize(-light.direction);
	float diff      = max(dot(normal, lightDir), 0.0f);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec      = pow(max(dot(viewDir, halfwayDir), 0.0f), material.shininess * 4.0f);
	
	vec3 ambient    = light.ambient * vec3(texture(material.diffuse, vs_out.uvcoord));
	vec3 diffuse    = light.diffuse * diff * vec3(texture(material.diffuse, vs_out.uvcoord));
	vec3 specular   = light.specular * spec * vec3(texture(material.specular, vs_out.uvcoord));
	float shadow = computeShadow(vs_out.fragpos_light_space, normal, lightDir);
	return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDir){
	float distance    = length(light.position - fragpos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 lightDir   = normalize(light.position - vec3(vs_out.fragpos));
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float diff = max(dot(normal, lightDir), 0.0f);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.shininess * 4.0f);

	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, vs_out.uvcoord));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, vs_out.uvcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vs_out.uvcoord));
	ambient  *= attenuation;
	diffuse  *= attenuation; 
	specular *= attenuation; 
	return (ambient + diffuse + specular);
}

float computeShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;
	// float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.01);  
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(depthMap, 0);
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