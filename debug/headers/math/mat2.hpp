#ifndef MAT2_H
#define MAT2_H
#include <iomanip>
#include <iostream>

template <class T>
struct mat2{
	mat2(){
		for(unsigned short i = 0; i < 2; ++i){
			for(unsigned short j = 0; j < 2; ++j){
				matrix[i][j] = (i == j)? 1: 0;
			}
		}
	}
	mat2 operator*(const mat2& m) const{
		mat2 result;
		for(unsigned short i = 0; i < 2; i++){
			for(unsigned short j = 0; j < 2; j++){
				result.matrix[i][j] = matrix[i][0] * m.matrix[0][j] + matrix[i][1] * m.matrix[1][j] + matrix[i][2] * m.matrix[2][j];
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
	//to work...
	mat2 inverse() const{
		mat2 identity;
		for(unsigned short column = 0; column < 2; column++){
	  		for(unsigned short row = 0; row < 2; row++){
				if(row != column){
					float coeff = matrix[row][column] / matrix[column][column];
					if(coeff != 0){
						for(unsigned short j = 0; j < 2; j++){
							matrix[row][j] -= coeff * matrix[column][j];
							identity.matrix[row][j] -= coeff * identity.matrix[column][j];
						}
						matrix[row][column] = 0.0;
					}
				}
			}		
		}
		for(unsigned short row = 0; row < 2; row++){ 
	    	for (unsigned short column = 0; column < 2; column++){ 
	       	 identity.matrix[row][column] /= matrix[row][row]; 
	    	} 
		} 
		return identity; 
	}
	T matrix[2][2];
};

#endif