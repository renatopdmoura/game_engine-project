#ifndef MAT4_H
#define MAT4_H
#include <string>
#include <iomanip>
#include <iostream>
#include <cmath>

template <class T>
struct mat4{
	mat4(){
		for(unsigned short i = 0; i < 4; ++i){
			for(unsigned short j = 0; j < 4; ++j){
				matrix[i][j] = (i == j)? 1: 0;
			}
		}
	}
	mat4 operator*(const mat4& m) const{
		mat4 result;
		for(unsigned short i = 0; i < 4; i++){
			for(unsigned short j = 0; j < 4; j++){
				result.matrix[i][j] = matrix[i][0] * m.matrix[0][j] + matrix[i][1] * m.matrix[1][j] + matrix[i][2] * m.matrix[2][j] + matrix[i][3] * m.matrix[3][j];
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
					T save_m[] = {m.matrix[j][0], m.matrix[j][1], m.matrix[j][2], m.matrix[j][3]};
					T save_n[] = {n.matrix[j][0], n.matrix[j][1], n.matrix[j][2], n.matrix[j][3]};
					for(unsigned k = 0; k < 4; ++k){
						m.matrix[j][k]     = matrix[big][k];
						m.matrix[big][k]   = *(save_m + k);
						n.matrix[j][k]     = n.matrix[big][k];
						n.matrix[big][k]   = *(save_n + k);
					}
				}
			}
			//Step 2
			for(unsigned i = 0; i < 4; ++i){
				if(i != j){
					T coeff = m.matrix[i][j] / m.matrix[j][j];
					if(coeff != 0.0f){
						for(unsigned k = 0; k < 4; ++k){
							m.matrix[i][j] -= coeff * m.matrix[j][k];
							n.matrix[i][j] -= coeff * n.matrix[j][k];
						}
						m.matrix[i][j] = 0.0f;
					}
				}
			}
		}
		for(unsigned i = 0; i < 4; ++i){
			for(unsigned j = 0; j < 4; ++j){
				n.matrix[i][j] /= m.matrix[i][i];
			}
		}
		
		//Step Two
		return n;
	}
	T matrix[4][4];
};

typedef mat4<float> mat4f;

#endif