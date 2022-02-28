#include "headers/link_function_test.hpp"
#include "test.hpp"

#include <iostream>
#include <string>

int main(){
	std::string error_msg;
	if(link_function_test()){
		error_msg = "Success...";
		output();
	}
	else{
		error_msg = "Failed...";
	}
	std::cout << error_msg << std::endl;
	return 0;
}