#ifndef MATH_H
#define MATH_H

#include <string>
#include <iomanip>
#include <iostream>
#include <cmath>

template <class T>
struct mat2{
	mat2(){
		for(unsigned short i = 0; i < 2; ++i){
			for(unsigned short j = 0; j < 2; ++j){
				matrix[i][j] = (i == j)? 1: 0;
			}
		}
	}
	const T* operator[](uint index) const{
		return matrix[index];
	}
	T* operator[](uint index){
		return matrix[index];
	}
	mat2 operator*(const mat2& m) const{
		mat2 result;
		for(unsigned short i = 0; i < 2; i++){
			for(unsigned short j = 0; j < 2; j++){
				result[i][j] = matrix[i][0] * m[0][j] + matrix[i][1] * m[1][j] + matrix[i][2] * m[2][j];
			}
		}
		return result; 
	}
	void show(std::string name) const{
		std::cout << std::setw(22) << name << ":" << std::endl;
		for(unsigned short i = 0; i < 2; ++i){
			for(unsigned short j = 0; j < 2; ++j){
				std::cout << std::setw(22) << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}

	T matrix[2][2];
};

template <class T>
struct mat4{
	mat4(){
		identity();
	}
	void identity(){
		for(unsigned short i = 0; i < 4; ++i){
			for(unsigned short j = 0; j < 4; ++j){
				matrix[i][j] = (i == j)? 1: 0;
			}
		}
	}
	const T* operator[](uint index) const{
		return matrix[index];
	}
	T* operator[](uint index){
		return matrix[index];
	}
	mat4 operator*(const mat4& m) const{
		mat4 result;
		for(unsigned short i = 0; i < 4; i++){
			for(unsigned short j = 0; j < 4; j++){
				result[i][j] = matrix[i][0] * m[0][j] + matrix[i][1] * m[1][j] + matrix[i][2] * m[2][j] + matrix[i][3] * m[3][j];
			}
		}
		return result; 
	}
	void show(std::string name) const{
		std::cout << std::setw(22) << name << ":" << std::endl;
		for(unsigned short i = 0; i < 4; ++i){
			for(unsigned short j = 0; j < 4; ++j){
				std::cout << std::setw(22) << matrix[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}
	mat4 transpose(){
		mat4 m;
		for(uint i = 0; i < 4; ++i){
			for(uint j = 0; j < 4; ++j){
				m[i][j] = matrix[j][i];
			}
		}
		return m;
	}

	mat4 inverse(){
		mat4 m, n;
		m = *this;
		for(unsigned j = 0; j < 4; ++j){
			//Step 1
			if(matrix[j][j] == 0.0f){
				unsigned big = j;
				for(unsigned i = 0; i < 4; ++i){
					if(fabs(matrix[i][j]) > fabs(matrix[big][j]))
						big = i;
				}
				if(big == j)
					std::cout <<"Singular matrix!" << std::endl;
				else{
					T save_m[] = {m[j][0], m[j][1], m[j][2], m[j][3]};
					T save_n[] = {n[j][0], n[j][1], n[j][2], n[j][3]};
					for(unsigned k = 0; k < 4; ++k){
						m[j][k]     = matrix[big][k];
						m[big][k]   = *(save_m + k);
						n[j][k]     = n[big][k];
						n[big][k]   = *(save_n + k);
					}
				}
			}
			//Step 2
			for(unsigned i = 0; i < 4; ++i){
				if(i != j){
					T coeff = m[i][j] / m[j][j];
					if(coeff != 0.0f){
						for(unsigned k = 0; k < 4; ++k){
							m[i][j] -= coeff * m[j][k];
							n[i][j] -= coeff * n[j][k];
						}
						m[i][j] = 0.0f;
					}
				}
			}
		}
		for(unsigned i = 0; i < 4; ++i){
			for(unsigned j = 0; j < 4; ++j){
				n[i][j] /= m[i][i];
			}
		}	
		return n;
	}

	T matrix[4][4];
};


template <class T>
struct vec2{
	vec2(){
		x = 0;
		y = 0;
	}
	vec2(T value){
		x = value;
		y = value; 
	}
	vec2(T a, T b){
		x = a;
		y = b;
	}
	vec2 operator-(const vec2& vector) const{
		return vec2(x - vector.x, y - vector.y);
	}
	vec2 operator+(const float value) const{
		return vec2(x + value, y + value);
	}
	void show(const char* name) const{
		std::cout << std::setw(10) << name << ":";
		std::cout << std::setw(10) << "[ " << x << " " << y << " ]" << std::endl;
	}

	union{
		T x;
		T r;
		T w;
		T u;
	};
	union{
		T y;
		T h;
		T g;
		T v;
	};
};


template<class T>
struct vec3 : public vec2<T>{
	vec3() : vec2<T>(){
		z = 0;
	}
	vec3(T value) : vec2<T>(value){
		z = value;
	}
	vec3(T value_x, T value_y, T value_z) : vec2<T>(value_x, value_y){
		z = value_z;
	}
	vec3(vec2<T>& obj){
		vec2<T>::x = obj.x;
		vec2<T>::y = obj.y;
		z = 0;
	} 
	vec3 operator-(const vec3& vector) const{
		return vec3(vec2<T>::x - vector.x, vec2<T>::y - vector.y, z - vector.z);
	}
	vec3 operator+(const vec3 vector) const{
		return vec3(vec2<T>::x + vector.x, vec2<T>::y + vector.y, z + vector.z);
	}
	vec3 operator*(const vec3& vector) const{
		vec3 result;
		result.x = vec2<T>::x * vector.x;
		result.y = vec2<T>::y * vector.y;
		result.z = z * vector.z;
		return result;
	}
	vec3 operator+(const T& value) const{
		return vec3(vec2<T>::x + value, vec2<T>::y + value, z + value);
	}
	vec3 operator-(const T& value) const{
		return vec3(vec2<T>::x - value, vec2<T>::y - value, z - value);
	}
	vec3 operator*(const T& value) const{
		vec3 result;
		result.x = vec2<T>::x * value;
		result.y = vec2<T>::y * value;
		result.z = z * value;
		return result;
	}
	vec3 operator/(const T& value) const{
		vec3 result;
		result.x = vec2<T>::x / value;
		result.y = vec2<T>::y / value;
		result.z = z / value;
		return result;
	}
	vec3 operator*(const mat4<T>& mat) const{
		vec3 result;
		result.x = vec2<T>::x * mat[0][0] + vec2<T>::y * mat[1][0] + z * mat[2][0] + mat[3][0];
		result.y = vec2<T>::x * mat[0][1] + vec2<T>::y * mat[1][1] + z * mat[2][1] + mat[3][1];
		result.z = vec2<T>::x * mat[0][2] + vec2<T>::y * mat[1][2] + z * mat[2][2] + mat[3][2];
		return result;
	}
	bool operator==(vec3 vector){
		return ((vec2<T>::x == vector.x && vec2<T>::y == vector.y) && z == vector.z);
	}
	bool operator<(vec3& vector){
		return (vec2<T>::x < vector.x) || (vec2<T>::y < vector.y) || (z < vector.z);
	}
	bool operator>(vec3& vector){
		return (vec2<T>::x > vector.x) || (vec2<T>::y > vector.y) || (z > vector.z);
	}
	void show(std::string name) const{
		std::cout << std::setw(10) << name << ":";
		std::cout << std::setw(10) << "[ " << vec2<T>::x << " " << vec2<T>::y << " " << z << " ]\n";
	}
	inline T* address(){
		return &(vec2<T>::x);
	}
	
	union{
		T z;
		T b;
	};
};

template <class T>
struct vec4 : public vec3<T>{
	vec4() : vec3<T>(){
		w = 1;
	}
	vec4(T value) : vec3<T>(value){
		w = value;
	}
	vec4(T value_x, T value_y, T value_z, T value_w = 1) : vec3<T>(value_x, value_y, value_z){
		w = value_w;
	}
	vec4(vec3<T>& obj, T value_w = 1){
		vec3<T>::x = obj.x;
		vec3<T>::y = obj.y;
		vec3<T>::z = obj.z;
		w = value_w;
	}
	vec4 operator-(const vec4& vector) const{
		return vec4(vec3<T>::x - vector.x, vec3<T>::y - vector.y, vec3<T>::z - vector.z, w - vector.w);
	}
	vec4 operator-(const T& value) const{
		return vec4(vec3<T>::x - value, vec3<T>::y - value, vec3<T>::z - value, w - value);
	}
	vec4 operator+(const vec4& vector) const{
		return vec4(vec3<T>::x + vector.x, vec3<T>::y + vector.y, vec3<T>::z + vector.z, w + vector.w);
	}
	vec4 operator+(const T& value) const{
		return vec4(vec3<T>::x + value, vec3<T>::y + value, vec3<T>::z + value, w + value);
	}
	vec4 operator*(const T& value) const{
		vec4<T> result;
		result.x = vec3<T>::x * value;
		result.y = vec3<T>::y * value;
		result.z = vec3<T>::z * value;
		result.w = w * value;
		return result;
	}
	vec4 operator/(const T& value) const{
		vec4<T> result;
		result.x = vec3<T>::x / value;
		result.y = vec3<T>::y / value;
		result.z = vec3<T>::z / value;
		result.w = w / value;
		return result;
	}
	vec4 operator*(const vec4& vector) const{
		vec4<T> result;
		result.x = vec3<T>::x * vector.x;
		result.y = vec3<T>::y * vector.y;
		result.z = vec3<T>::z * vector.z;
		result.w = w * vector.w;
		return result;
	}
	vec4 operator*(const mat4<T>& mat) const{
		vec4 result;
		result.x = vec3<T>::x * mat[0][0] + vec3<T>::y * mat[1][0] + vec3<T>::z * mat[2][0] +  w * mat[3][0];
		result.y = vec3<T>::x * mat[0][1] + vec3<T>::y * mat[1][1] + vec3<T>::z * mat[2][1] +  w * mat[3][1];
		result.z = vec3<T>::x * mat[0][2] + vec3<T>::y * mat[1][2] + vec3<T>::z * mat[2][2] +  w * mat[3][2];
		result.w = vec3<T>::x * mat[0][3] + vec3<T>::y * mat[1][3] + vec3<T>::z * mat[2][3] +  w * mat[3][3];
		vec4 out;
		out.x = result.x != 0.0f? result.x / result.w: 0.0f;
		out.y = result.y != 0.0f? result.y / result.w: 0.0f;
		out.z = result.z != 0.0f? result.z / result.w: 0.0f;
		out.w = w != 0.0f? result.w / result.w: 0.0f;
		return out;
	}
	void show(std::string name) const{
		std::cout << std::setw(10) << name << ":";
		std::cout << std::setw(10) << "[ " << vec3<T>::x << " " << vec3<T>::y << " " << vec3<T>::z << " " << w << " ]\n";
	}

	union{
		T w;
		T a;
	};
};

typedef vec2<float> vec2f;
typedef vec2<double> vec2d;
typedef vec2<int> vec2i;
typedef vec2<unsigned int> vec2ui;
typedef vec2<unsigned char> vec2uc;
typedef vec3<float> vec3f;
typedef vec3<double> vec3d;
typedef vec3<int> vec3i;
typedef vec3<unsigned int> vec3ui;
typedef vec3<unsigned char> vec3uc;
typedef vec4<float> vec4f;
typedef vec4<int> vec4i;
typedef vec4<unsigned int> vec4ui;
typedef vec4<unsigned char> vec4uc;
typedef mat2<float> mat2f;
typedef mat2<double> mat2d;
typedef mat4<float> mat4f;
typedef mat4<double> mat4d;

const float PI = 3.141516f;

template<typename  T>
T radians(T angle){
	return ((angle * PI) / 180.0);
}

template<typename T>
vec3<T> normalize(const vec3<T>& vector){
	T module = sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
	if(!module){
		std::cout << "Null vector! Module is 0." << std::endl;
	}
	return vec3<T>((vector.x / module), (vector.y / module), (vector.z / module));
}

template<typename T>
vec3<T> cross(const vec3<T>& a, const vec3<T>& b){
	return vec3<T>((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x));
}

template<typename T>
T dot(const vec3<T>& a, const vec3<T>& b){
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

template<typename T>
T length(vec3<T>& a, vec3<T>& b){
	vec3<T> major, minor;
	if(a > b){
		major = a;
		minor = b;
	}
	else{
		major = b;
		minor = a;
	}
	vec3<float> vector = major - minor;
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

template<typename T>
mat4<T> lookAt(const vec3<T>& eye, const vec3<T>& target, const vec3<T> up){
	vec3<T> zaxis = normalize(target - eye);
	vec3<T> xaxis = normalize(cross(zaxis, up));
	vec3<T> yaxis = cross(xaxis, zaxis);
	mat4<T> result;
	result[0][0] = xaxis.x;
	result[1][0] = xaxis.y;
	result[2][0] = xaxis.z;
	result[0][1] = yaxis.x;
	result[1][1] = yaxis.y;
	result[2][1] = yaxis.z;
	result[0][2] = -zaxis.x;
	result[1][2] = -zaxis.y;
	result[2][2] = -zaxis.z;
	result[3][0] = -dot(xaxis, eye);
	result[3][1] = -dot(yaxis, eye);
	result[3][2] = dot(zaxis, eye);
	return result;
}

template<typename T>
mat4<T> perspective(T fovy, T aspect, T zNear, T zFar){
	mat4<T> result;
	T tanHalfFovy = tan(radians(fovy) / 2.0f);
	T t = tanHalfFovy * zNear;
	T b = -t;
	T r = t * aspect;
	T l = b * aspect;
	result[0][0] = (2.0f * zNear) / (r - l);
	// result.matrix[2][0] = (r + l) / (r - l);
	result[1][1] = (2.0f * zNear) / (t - b);
	// result.matrix[2][1] = (t + b) / (t - b);

	//Transforma a coordenada z do intervalo do frustum de visualização
	//para valores não lineares de proporção 1/z
	result[2][2] = -(zFar + zNear) / (zFar - zNear);
	result[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
	
	result[2][3] = -1.0f;
	result[3][3] = 0.0f;
	return result;
}

template<typename T>
mat4<T> perspective(T angle, T w, T h){
	mat4<T> result;
	T near   = 0.1f;
	T far    = 1.0f;
	T aspect = w / h;

	//1° Defini o ângulo de visão
	T tanHalfFovy = tan(radians(angle) / 2);
	result[0][0] = 1.0f / (aspect * tanHalfFovy);
	result[1][1] = 1.0f / tanHalfFovy;

	//2° Negar o fator de escala z e mapea-lô para [near, far]
	result[2][2] = - (far / far - near);
	result[3][2] = - (far * near / (far - near));

	//3° Definir w = -z
	result[2][3] = -1.0f;

	//Zero, ou o plano de recorte próximo será indefinido - ainda não compreendo
	result[3][3] = 0.0f;
	return result;
}

template<typename T>
mat4<T> orthographic(T l, T r, T b, T t, T near, T far){
	mat4<T> result;
	result[0][0] = 2 / (r - l);
	result[1][1] = 2 / (t - b);
	result[2][2] = -2.0f / (far - near);
	result[3][0] = - (r + l) / (r - l);
	result[3][1] = - (t + b) / (t - b);
	result[3][2] = - (far + near) / (far - near);
	result[3][3] = 1.0f;
	return result;
}

template<typename T>
mat4<T> orthographic(T l, T r, T b, T t){
	mat4<T> result;
	result[0][0] = 2 / (r - l);
	result[1][1] = 2 / (t - b);
	result[2][2] = -1;
	result[3][0] = - (r + l) / (r - l);
	result[3][1] = - (t + b) / (t - b);
	return result;
}
template<typename T>
mat4<T> translate(vec3<T> v){
	mat4<T> matrix;
	matrix[3][0] += v.x;
	matrix[3][1] += v.y;
	matrix[3][2] += v.z;
	return matrix;
}
template<typename T>
mat4<T> scale(vec3<T> v){
	mat4<T> matrix;
	matrix[0][0] *= v.x;
	matrix[1][1] *= v.y;
	matrix[2][2] *= v.z;
	return matrix;
}

template<typename T>
mat4<T> rotateX(T angle){
	mat4<T> matrix;
	matrix[1][1] = cos(radians(angle));
	matrix[2][1] = sin(radians(angle));
	matrix[1][2] = -sin(radians(angle));
	matrix[2][2] = cos(radians(angle));
	return matrix;
}

template<typename T>
mat4<T> rotateY(T angle){
	mat4<T> matrix;
	matrix[0][0] = cos(radians(angle));
	matrix[2][0] = -sin(radians(angle));
	matrix[2][2] = cos(radians(angle));
	matrix[0][2] = sin(radians(angle));
	return matrix;
}

template<typename T>
mat4<T> rotateZ(T angle){
	mat4<T> matrix;
	matrix[0][0] = cos(radians(angle));
	matrix[1][0] = -sin(radians(angle));
	matrix[0][1] = sin(radians(angle));
	matrix[1][1] = cos(radians(angle));
	return matrix;
}
#endif