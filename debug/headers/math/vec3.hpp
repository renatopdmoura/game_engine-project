#ifndef VEC3_H
#define VEC3_H
#include "mat4.hpp"
#include <string>
#include <iomanip>
#include <iostream>

template <class T>
struct vec3{
	vec3(){
		x = 0;
		y = 0;
		z = 0;
	}
	vec3(T value){
		x = value;
		y = value;
		z = value;
	}
	vec3(T value_x, T value_y, T value_z){
		x = value_x;
		y = value_y;
		z = value_z;
	}
	vec3 operator-(const vec3& vector) const{
		vec3 temp(this->x - vector.x, this->y - vector.y, this->z - vector.z);
		return temp;
	}
	vec3 operator+(const vec3 vector) const{
		vec3 temp(this->x + vector.x, this->y + vector.y, this->z + vector.z);
		return temp;
	}
	vec3 operator+(const T& value) const{
		vec3 temp(this->x + value, this->y + value, this->z + value);
		return temp;
	}
	vec3 operator-(const T& value) const{
		vec3 temp(this->x - value, this->y - value, this->z - value);
		return temp;
	}
	vec3 operator*(const T& value) const{
		vec3 result;
		result.x = x * value;
		result.y = y * value;
		result.z = z * value;
		return result;
	}
	vec3 operator/(const T& value) const{
		vec3 result;
		result.x = x / value;
		result.y = y / value;
		result.z = z / value;
		return result;
	}
	vec3 operator*(const vec3& vector) const{
		vec3 result;
		result.x = x * vector.x;
		result.y = y * vector.y;
		result.z = z * vector.z;
		return result;
	}
	vec3 operator*(const mat4<T>& mat) const{
		vec3 result;
		result.x = x * mat.matrix[0][0] + y * mat.matrix[1][0] + z * mat.matrix[2][0] + mat.matrix[3][0];
		result.y = x * mat.matrix[0][1] + y * mat.matrix[1][1] + z * mat.matrix[2][1] + mat.matrix[3][1];
		result.z = x * mat.matrix[0][2] + y * mat.matrix[1][2] + z * mat.matrix[2][2] + mat.matrix[3][2];
		return result;
	}
	bool operator==(vec3 vector){
		return ((x == vector.x && y == vector.y) && z == vector.z);
	}
	bool operator<(vec3& vector){
		return (x < vector.x) || (y < vector.y) || (z < vector.z);
	}
	bool operator>(vec3& vector){
		return (x > vector.x) || (y > vector.y) || (z > vector.z);
	}
	void show(std::string name) const{
		std::cout << std::setw(10) << name << ":";
		std::cout << std::setw(10) << "[ " << x << " " << y << " " << z << " ]\n";
	}
	T sum(){
		return x + y + z;
	}
	inline T* address(){
		return &x;
	}
	union{
		T x;
		T r;
	};
	union{
		T y;
		T g;
	};
	union{
		T z;
		T b;
	};
};

typedef vec3<double> vec3d;
typedef vec3<float> vec3f;
typedef vec3<int> vec3i;
typedef vec3<unsigned int> vec3uint;
typedef vec3<unsigned char> vec3uchar;

template <class T>
class vec2{
public:
	vec2(){
		x = 0;
		y = 0;
	}
	vec2(T u){
		x = u;
		y = u; 
	}
	vec2(T u, T v){
		x = u;
		y = v;
	}
	vec2 operator-(const vec2& vector) const{
		vec2 temp(this->x - vector.x, this->y - vector.y);
		return temp;
	}
	void show(const char* name) const{
		std::cout << std::setw(10) << name << ":";
		std::cout << std::setw(10) << "[ " << int(x) << " " << int(y) << " ]\n";
	}
	union{
		T x;
		T w;
	};
	union{
		T y;
		T h;
	};
};

typedef vec2<double> vec2d;
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;
typedef vec2<unsigned int> vec2uint;
typedef vec2<unsigned char> vec2uchar;

#endif