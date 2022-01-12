#ifndef VEC4_H
#define VEC4_H
#include "vec3.hpp"

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
		vec4 temp(this->x - vector.x, this->y - vector.y, this->z - vector.z, this->w - vector.w);
		return temp;
	}
	vec4 operator-(const T& value) const{
		vec4 temp(this->x - value, this->y - value, this->z - value, this->w - value);
		return temp;
	}
	vec4 operator+(const vec4& vector) const{
		vec4 temp(this->x + vector.x, this->y + vector.y, this->z + vector.z, this->w + vector.w);
		return temp;
	}
	vec4 operator+(const T& value) const{
		vec4 temp(this->x + value, this->y + value, this->z + value, this->w + value);
		return temp;
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
		result.x = vec3<T>::x * mat.matrix[0][0] + vec3<T>::y * mat.matrix[1][0] + vec3<T>::z * mat.matrix[2][0] +  w * mat.matrix[3][0];
		result.y = vec3<T>::x * mat.matrix[0][1] + vec3<T>::y * mat.matrix[1][1] + vec3<T>::z * mat.matrix[2][1] +  w * mat.matrix[3][1];
		result.z = vec3<T>::x * mat.matrix[0][2] + vec3<T>::y * mat.matrix[1][2] + vec3<T>::z * mat.matrix[2][2] +  w * mat.matrix[3][2];
		result.w = vec3<T>::x * mat.matrix[0][3] + vec3<T>::y * mat.matrix[1][3] + vec3<T>::z * mat.matrix[2][3] +  w * mat.matrix[3][3];
		vec4 out;
		out.x = result.vec3<T>::x != 0.0f? result.x / result.w: 0.0f;
		out.y = result.vec3<T>::y != 0.0f? result.y / result.w: 0.0f;
		out.z = result.vec3<T>::z != 0.0f? result.z / result.w: 0.0f;
		out.w = w != 0.0f? result.w / result.w: 0.0f;
		return out;
	}
	void show(std::string name) const{
		std::cout << std::setw(10) << name << ":";
		std::cout << std::setw(10) << "[ " << vec3<T>::x << " " << vec3<T>::y << " " << vec3<T>::z << " " << w << " ]\n";
	}
	union{
		T w;
	};
};

typedef vec4<float> vec4f;
typedef vec4<int> vec4i;
typedef vec4<unsigned int> vec4uint;
typedef vec4<unsigned char> vec4uchar;

#endif