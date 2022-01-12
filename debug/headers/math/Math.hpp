#ifndef MATH
#define MATH
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat2.hpp"
#include "mat4.hpp"
#include <cmath>

const float PI = 3.141516f;

extern float radians(const float& angle){
	return ((angle * PI) / 180.0);
}

extern float clamp(const float& v, const float& min, const float& max){
	return (v < min? min: max < v? max: v);
}

extern bool equalf(vec3f& A, vec3f& B){
	return (A.x == B.x && A.y == B.y && A.z == B.z);
}

extern bool notequalf(vec3f& A, vec3f& B){
	return (A.x != B.x || A.y != B.y || A.z != B.z);
}

extern vec3f normalize(const vec3f& vector){
	float module = sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
	if(!module){
		std::cout << "Null vector! Module is 0." << std::endl;
	}
	vec3f temp((vector.x / module), (vector.y / module), (vector.z / module));
	return temp;
}

extern vec3f cross(const vec3f& a, const vec3f& b){
	vec3f temp;
	temp.x = (a.y * b.z) - (a.z * b.y);
	temp.y = (a.z * b.x) - (a.x * b.z);
	temp.z = (a.x * b.y) - (a.y * b.x);
	return temp;
}

extern float dot(const vec3f& a, const vec3f& b){
	return float(a.x * b.x + a.y * b.y + a.z * b.z);
}

extern float dot(const vec2f& a, const vec2f& b){
	return float(a.x * b.x + a.y * b.y);
}

extern mat4<float> orientation(mat4<float> mat){
	mat.matrix[3][0] = 0.0f;
	mat.matrix[3][1] = 0.0f;
	mat.matrix[3][2] = 0.0f;
	mat.matrix[0][3] = 0.0f;
	mat.matrix[1][3] = 0.0f;
	mat.matrix[2][3] = 0.0f;
	mat.matrix[3][3] = 1.0f;
	return mat;
}

extern mat4<float> translate(const vec3f& vector){
	mat4<float> mat;
	mat.matrix[3][0] = vector.x;
	mat.matrix[3][1] = vector.y;
	mat.matrix[3][2] = vector.z;
	return mat;
}

extern mat4<float> scale(const vec3f& vector){
	mat4<float> mat;
	mat.matrix[0][0] = vector.x;
	mat.matrix[1][1] = vector.y;
	mat.matrix[2][2] = vector.z;
	return mat;
}

extern mat4<float> rotateX(const float& pitch){
	mat4<float> xaxis;
	xaxis.matrix[1][1] = cos(radians(pitch));
	xaxis.matrix[2][1] = sin(radians(pitch));
	xaxis.matrix[1][2] = -sin(radians(pitch));
	xaxis.matrix[2][2] = cos(radians(pitch));
	return xaxis;
}

extern mat4<float> rotateY(const float& yaw){
	mat4<float> yaxis;
	yaxis.matrix[0][0] = cos(radians(yaw));
	yaxis.matrix[2][0] = -sin(radians(yaw));
	yaxis.matrix[2][2] = cos(radians(yaw));
	yaxis.matrix[0][2] = sin(radians(yaw));
	return yaxis;
}

extern mat4<float> rotateZ(const float& roll){
	mat4<float> zaxis;
	zaxis.matrix[0][0] = cos(radians(roll));
	zaxis.matrix[1][0] = -sin(radians(roll));
	zaxis.matrix[0][1] = sin(radians(roll));
	zaxis.matrix[1][1] = cos(radians(roll));
	return zaxis;
}

extern mat2<float> rotateZ_2D(const float& roll){
	 mat2<float> zaxis;
	 zaxis.matrix[0][0] = cos(radians(roll));
	 zaxis.matrix[1][0] = -sin(radians(roll));
	 zaxis.matrix[0][1] = sin(radians(roll));
	 zaxis.matrix[1][1] = cos(radians(roll));
	 return zaxis;
}

extern mat4<float> lookAtRH(const vec3f& eye, const vec3f& target, const vec3f up){
	vec3f zaxis = normalize(target - eye);
	vec3f xaxis = normalize(cross(zaxis, up));
	vec3f yaxis = cross(xaxis, zaxis);

	mat4<float> result;
	//Incrmenta a unidade lógica de deslocamento rotacional
	result.matrix[0][0] = xaxis.x;
	result.matrix[1][0] = xaxis.y;
	result.matrix[2][0] = xaxis.z;
	result.matrix[0][1] = yaxis.x;
	result.matrix[1][1] = yaxis.y;
	result.matrix[2][1] = yaxis.z;
	//Nega-se o vetor z para que a função seno comporte-se de forma esperada
	//ao olhar para cima e para baixo (recuando e avançando)
	result.matrix[0][2] = -zaxis.x;
	result.matrix[1][2] = -zaxis.y;
	result.matrix[2][2] = -zaxis.z;
	//O sentido da direção é do alvo ao "olho"
	//Portanto, ao incrementar ou decrementar o "olho", afasta-se os objetos no mundo
	//O alvo é ponto fixo para definir a distância entre dois pontos, variando-a 
	//pelo deslocamento do "olho".
	//Nega-se o compontente x de tradução para simular o efeito de câmera deslocando-se para direita
	//ja que na verdade o efeito real é deslocar o mundo para esquerda e vice-versa.
	result.matrix[3][0] = -dot(xaxis, eye);
	//Nega-se o componente y de tradução para simular o efeito de câmera deslocando-se para cima
	//já que na verdade o efeito real é o mundo descendo e vice-versa.
	result.matrix[3][1] = -dot(yaxis, eye);
	//A coordenada z não precisa ser negada pois valores positivos simulam um avanço e negativos recuo
	//de forma que valores positivo estão fora do campo de visualização e valores negativos são visíveis 
	//já que nossa matriz segue a definição OpenGL do espaço de clip - -z para dentro da tela.
	result.matrix[3][2] = dot(zaxis, eye);

	return result;
}

extern mat4<float> orthographic(float r, float l, float t, float b, float near, float far){
	mat4<float> ortho;
	ortho.matrix[0][0] = 2 / (r - l);
	ortho.matrix[1][1] = 2 / (t - b);
	ortho.matrix[2][2] = -2.0f / (far - near);
	ortho.matrix[3][0] = - (r + l) / (r - l);
	ortho.matrix[3][1] = - (t + b) / (t - b);
	ortho.matrix[3][2] = - (far + near) / (far - near);
	ortho.matrix[3][3] = 1.0f;
	return ortho;
}

extern mat4<float> ortho(float r, float l, float t, float b){
	mat4<float> ortho;
	ortho.matrix[0][0] = 2 / (r - l);
	ortho.matrix[1][1] = 2 / (t - b);
	ortho.matrix[2][2] = -1;
	ortho.matrix[3][0] = - (r + l) / (r - l);
	ortho.matrix[3][1] = - (t + b) / (t - b);
	return ortho;
}

extern mat4<float> perspective(float angle, int w, int h){
	mat4<float> perspectiveMatrix;
	float near   = 0.1f;
	float far    = 1.0f;
	float aspect = (float)w / (float)h;

	//1° Defini o ângulo de visão
	float tanHalfFovy = tan(radians(angle) / 2);
	perspectiveMatrix.matrix[0][0] = 1.0f / (aspect * tanHalfFovy);
	perspectiveMatrix.matrix[1][1] = 1.0f / tanHalfFovy;

	//2° Negar o fator de escala z e mapea-lô para [near, far]
	perspectiveMatrix.matrix[2][2] = - (far / far - near);
	perspectiveMatrix.matrix[3][2] = - (far * near / (far - near));

	//3° Definir w = -z
	perspectiveMatrix.matrix[2][3] = -1.0f;

	//Zero, ou o plano de recorte próximo será indefinido - ainda não compreendo
	perspectiveMatrix.matrix[3][3] = 0.0f;
	return perspectiveMatrix;
}

//Converte as coordenadas para espaço de clipe (tronco de pirâmide) e prepara a coordenada w para divisão da pespectiva (converter para NDC - espaço unitário)
extern mat4<float> perspective(float fovy, float aspect, float zNear, float zFar){
	mat4<float> result;
	float tanHalfFovy = tan(radians(fovy) / 2.0f);
	float t = tanHalfFovy * zNear;
	float b = -t;
	float r = t * aspect;
	float l = b * aspect;
	result.matrix[0][0] = (2.0f * zNear) / (r - l);
	// result.matrix[2][0] = (r + l) / (r - l);
	result.matrix[1][1] = (2.0f * zNear) / (t - b);
	// result.matrix[2][1] = (t + b) / (t - b);
	result.matrix[2][2] = -(zFar + zNear) / (zFar - zNear);
	result.matrix[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
	result.matrix[2][3] = -1.0f;
	result.matrix[3][3] = 0.0f;
	return result;
}
#endif