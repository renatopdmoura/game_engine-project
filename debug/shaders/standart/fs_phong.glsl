#version 460
#define NR_POINT_LIGHTS 4

in struct VS_OUT{
	vec3 fragpos;
	vec3 normal;
	vec2 uvcoord;
	vec4 fragPosLightSpace;
	mat3 TBN;
}vs_out;

out vec4 fragment;

uniform struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	// sampler2D depth;
	// sampler2D emissive;
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

uniform float zFar;
uniform vec3 camerapos;
uniform sampler2D depthMap;
uniform samplerCube depthCubemap;
uniform float sHeight;

vec2 gTexCoords;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir){
	//Quantidade de camadas com base no ângulo de visualização
	const float minNumLayers = 8.0f;
	const float maxNumLayers = 32.0f;
	const float numLayers    = mix(maxNumLayers, minNumLayers, max(dot(vec3(0.0f, 0.0f, 1.0f), viewDir), 0.0f));
	
	//Profundidade por camada (linear)
	float layerDepth         = 1.0f / numLayers;
	float currentLayerDepth  = 0.0f;
	
	//A distância do fragmento ao visualizador
	vec2 surfacePoint        = viewDir.xy / viewDir.z * sHeight;
	
	//A coordenada de textura para o fragmento atual e a taxa de deslocamentos de coordenadas (linear)
	vec2 currentTexCoords    = vs_out.uvcoord;
	vec2 deltaTexCoords      = surfacePoint / numLayers;
	
	//Obtém a profundidade do mapa de profundidade para o fragmento respectivo
	float currentDepthMapValue = texture(material.depth, currentTexCoords).r;
	
	//Varrendo as camadas para determinar o vetor "P" mais próximo do ponto B
	while(currentLayerDepth < currentDepthMapValue){
		currentTexCoords    -= deltaTexCoords;
		currentDepthMapValue = texture(material.depth, currentTexCoords).r;
		currentLayerDepth   += layerDepth;
	}

	//Interpola entre as coordenadas de textura anteriores a intersecção e atuais
	vec2 prevTexCoords  = currentTexCoords + deltaTexCoords;
	float afterDepth    = currentDepthMapValue - currentLayerDepth;
	float beforeDepth   = texture(material.depth, prevTexCoords).r - currentLayerDepth + layerDepth;
	float weight        = afterDepth / (afterDepth - beforeDepth); 
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);
	return finalTexCoords;
}

float computeDirectionalShadow(vec4 fragPosLightSpace){
	vec3 projCoords    = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5f + 0.5f;
	float closesDepth  = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias 		   = max(0.05f * (1.0f - dot(vs_out.normal, normalize(-directionalLight.direction))), 0.005f);
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

float computePonctualShadow(PointLight light){
	vec3 fragToLight   = vs_out.fragpos - light.position;
	float closestDepth  = texture(depthCubemap, fragToLight).r;
	closestDepth *= zFar;
	float currentDepth = length(fragToLight);
	float bias   = 0.05f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f: 0.0f;
	return shadow;
}

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	//Ambient
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, gTexCoords));
	
	//Diffuse
	vec3 lightDir = vs_out.TBN * normalize(-light.direction);
	float diff    = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, gTexCoords));

	//Specular

	// Phong
	// vec3 reflectDir = reflect(-lightDir, normal);
	// float spec  	= pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);

	//Blinn-Phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec      = pow(max(dot(halfwayDir, normal), 0.0f), material.shininess);
	
	vec3 specular   = light.specular * spec * vec3(texture(material.specular, gTexCoords));

	float shadow = computeDirectionalShadow(vs_out.fragPosLightSpace);
	return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 viewDir){
	//Ambient
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, gTexCoords));

	//Diffuse
	vec3 lightDir = vs_out.TBN * normalize(light.position - vs_out.fragpos);
	float diff    = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, gTexCoords));

	//Specular
	
	//Phong
	// vec3 reflectDir = reflect(-lightDir, vs_out.normal);
	// float spec 		= pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	
	//Blinn-Phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec      = pow(max(dot(halfwayDir, normal), 0.0f), material.shininess);
	vec3 specular   = light.specular * spec * vec3(texture(material.specular, gTexCoords));

	//Atenuation
	float distance    = length(light.position - vs_out.fragpos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	float shadow = computePonctualShadow(pointLight[0]);
	return (ambient + (1.0f - shadow) * (diffuse + specular));
	// return (ambient + diffuse + specular);
}

void main(){
	vec3 normal = texture(material.normal, vs_out.uvcoord).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	vec3 viewDir = normalize((vs_out.TBN * camerapos) - (vs_out.TBN * vs_out.fragpos));
	// gTexCoords = parallaxMapping(vs_out.uvcoord, viewDir);
	gTexCoords = vs_out.uvcoord;

	vec3 result = computeDirectionalLight(directionalLight, normal, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; ++i){
		result += computePointLight(pointLight[i], normal, viewDir);
	}
	
	//Gama correction method 2
	float gamma = 2.2f;
	fragment.rgba = vec4(pow(result.rgb, vec3(1.0f / gamma)), 1.0f);
}
