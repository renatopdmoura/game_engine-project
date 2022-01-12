#version 460 core

#define NR_POINT_LIGHTS 4 

out vec4 fragment;
in vec2 uvcoord;

uniform struct Textures{
	sampler2D position;
	sampler2D normal;
	sampler2D albedo;
}gBuffer;

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

uniform sampler2D texDirShadowMap;
uniform mat4 lightSpaceMatrix;
uniform vec3 camerapos;

vec4 fragPosLightSpace;

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	//compute global light color
	vec3 ambient    = light.ambient * texture(gBuffer.albedo, uvcoord).rgb;
	vec3 lightDir   = normalize(-light.direction);
	float diff      = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse    = light.diffuse * diff * texture(gBuffer.albedo, uvcoord).rgb;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec      = pow(max(dot(halfwayDir, normal), 0.0f), texture(gBuffer.position, uvcoord).a);
	vec3 specular   = light.specular * spec * texture(gBuffer.albedo, uvcoord).a;
	
	//compute global shadow
	vec3 projCoords    = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5f + 0.5f;
	float closesDepth  = texture(texDirShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias 		   = max(0.05f * (1.0f - dot(normal, normalize(-light.direction))), 0.005f);
	float shadow 	   = 0.0f;
	vec2 texelSize	   = 1.0f / textureSize(texDirShadowMap, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(texDirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth? 1.0f: 0.0f;
		}
	}
	shadow /= 9.0f;
	if(projCoords.z > 1.0f) 
		shadow = 0.0f;

	//return light color + shadow
	return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragpos){
	vec3 ambient    = light.ambient * texture(gBuffer.albedo, uvcoord).rgb;
	vec3 lightDir   = normalize(light.position - fragpos);
	float diff      = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse    = light.diffuse * diff * texture(gBuffer.albedo, uvcoord).rgb;	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec 		= pow(max(dot(halfwayDir, normal), 0.0f), texture(gBuffer.position, uvcoord).a);
	vec3 specular   = light.specular * spec * texture(gBuffer.albedo, uvcoord).a;
	
	float distance  = length(light.position - fragpos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main(){
	vec3 fragpos = texture(gBuffer.position, uvcoord).rgb;
	fragPosLightSpace = lightSpaceMatrix * vec4(fragpos, 1.0f);
	vec3 normal  = texture(gBuffer.normal, uvcoord).rgb;	
	vec3 viewDir = normalize(camerapos - fragpos);
	vec3 result  = computeDirectionalLight(directionalLight, normal, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
		result += computePointLight(pointLight[i], normal, viewDir, fragpos);
	
	float gamma = 2.2f;
	fragment.rgba = vec4(pow(result.rgb, vec3(1.0f / gamma)), 1.0f);
}