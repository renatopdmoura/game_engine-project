#version 460
smooth in vec3 out_color;
in vec3 out_normal;
in vec2 out_uvcoord;
in vec3 out_fragpos;
out vec4 out_fragcolor;

struct Light{
	vec3 position;
};

uniform Light light;
uniform vec3 camerapos;

void main(){
	vec3 objectColor       = vec3(1.0f, 0.5f, 0.31f);
	vec3 lightColor        = vec3(1.0f, 1.0f, 1.0f);
	float ambientIntensity = 0.1f;
	vec3 ambient           = ambientIntensity * lightColor;

	vec3 normal   = normalize(out_normal);
	vec3 lightDir = normalize(light.position - out_fragpos);
	float diff    = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse  = diff * lightColor;

	float specularItensity = 0.5f;
	vec3 viewDir    = normalize(camerapos - out_fragpos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec      = pow(max(dot(viewDir, reflectDir), 0.0f), 128.0f);
	vec3 specular   = specularItensity * spec * lightColor;

	vec3 result   = (ambient + diffuse + specular) * objectColor;
	out_fragcolor = vec4(result, 1.0f);
}

/*
O produto escalar entre dois vetores unitários retorna o cosseno do ângulo entre eles.
Quanto menor o ângulo, mais o resultado aproxima-se de 1, quanto maior o ângulo inclina-se para 0,
valores acima de 90° resultam em valores negativos por isso deve-se fixar o valor no intervalo [0, 1].
Após, usar o valor obtido como fator de intensidade diffusa.
*/