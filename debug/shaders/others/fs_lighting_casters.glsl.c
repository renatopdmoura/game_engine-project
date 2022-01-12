#version 460
smooth in vec3 out_color;
in vec2 out_uvcoord;
in vec3 out_normal;
in vec3 out_fragpos;
out vec4 out_fragcolor;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout(location = 12) uniform Material material;
layout(location = 16) uniform Light light;
layout(location = 20) uniform vec3 camerapos;

void main(){
	//ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, out_uvcoord));

	//diffuse
	vec3 normal   = normalize(out_normal);
	vec3 lightDir = normalize(-light.direction);
	float diff    = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, out_uvcoord));

	//specular
	vec3 viewDir    = normalize(camerapos - out_fragpos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec      = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular   = light.specular * spec * vec3(texture(material.specular, out_uvcoord));

	vec3 result   = (ambient + diffuse + specular);
	out_fragcolor = vec4(result, 1.0f);
}