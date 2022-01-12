#version 460

in struct VS_OUT{
 vec3 normal;
 vec3 fragpos;
 vec3 lightPos;
 vec3 direction;
}vs_out;

out vec4 fragment;

void main(){
	//Ambient impact
	vec3 lightColor   = vec3(1.0f, 1.0f, 1.0f);
	vec3 objectColor  = vec3(0.8f, 0.3f, 0.6f);
	float ambStrength = 0.2f;
	vec3 ambient      = lightColor * ambStrength;

	//Global Light
	// vec3 lightDir = normalize(-vs_out.direction);
	//Local Light
	vec3 lightDir = normalize(vs_out.lightPos - vs_out.fragpos);
	float phi     = 0.86f;
	float gama    = 0.75f;
	float epsilon = phi - gama;
	float theta   = dot(normalize(-vs_out.direction), lightDir);
	float lerp    = clamp((theta - gama) / epsilon, 0.0f, 1.0f);

	// if(theta > gama)
	//Diffuse impact
	float distance    = length(vs_out.lightPos - vs_out.fragpos);
	float constant    = 1.0f;
	float linear      = 0.09f;
	float quadratic   = 0.032;
	float attenuation = 1.0f /(constant + linear * distance + quadratic * (distance * distance)); 
			
	float diff    = max(dot(lightDir, vs_out.normal), 0.0f);
	vec3 diffuse  = lightColor * diff;

	//Specular impact
	vec3 viewDir       = normalize(-vs_out.fragpos);
	vec3 reflectDir    = reflect(-lightDir, vs_out.normal);
	float spec 		   = pow(max(dot(viewDir, reflectDir), 0.0f), 64.0f);
	float specStrength = 0.8f;
	vec3 specular      = lightColor * spec * specStrength;

	//If use local light
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	diffuse  *= lerp;
	specular *= lerp;

	//Output
	fragment = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}