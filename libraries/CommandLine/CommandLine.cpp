#include "CommandLine.hpp"

CommandLine::CommandLine(Material* mtl, Text& in){
	material = mtl;
	input    = &in;
	elapsedTime = 0.0f;
}

inline bool CommandLine::isFloatFormat(std::string& value){
	std::string::iterator it;
	uint dot_count = 0;
	for(it = value.begin(); it < value.end(); ++it){
		if(std::isdigit(*it))
			continue;
		else if
			(*it == '.') dot_count++;
		else{
			strLog = "Invalid format";
			return false;
		} 
	}
	if(dot_count == 1 || dot_count == 0){
		return true;
	}
	else{
		strLog = value + " is invalid!";
		return false;
	}
}

void CommandLine::setMaterial(std::string command){
	std::string root = "/:";
	uint i = 0;
	while(i < command.length() && command[i] != '.'){
		root += command[i];
		++i;
	}
	std::string branch;
	while(i < command.length() && command[i] != ' '){
		branch += command[i];
		++i;
	}
	if(root + branch == "/:material.albedo"){
		bool success = true;
		uint count  = 0;
		uint offset = 0;
		std::vector<float*> adress = {&material->albedo.r, &material->albedo.g, &material->albedo.b};
		vec3<float> components = vec3<float>(material->albedo.r, material->albedo.g, material->albedo.b);
		std::string value("");
		while(i < command.length()){
			if(command[i] == ' ') ++i;
			while(command[i] != ' '){
				if(i < command.length()){
					value += command[i];
					++i;
				}
				else break;
			}
			success = isFloatFormat(value);
			if(success){
				count++;
				if(count <= 3){
					*adress[offset] = std::stof(value);
					++offset;
				}
			}
			else{
				material->albedo = components;
				break;
			}
			value.clear();
		}
		std::cout << count << std::endl;
		if(count > 3){
			strLog  = "Too many specified values!";
			success = false;
		}
		if(success)
			strLog = root + branch + " " + std::to_string(*(&material->albedo.r)) + " " + std::to_string(*(&material->albedo.g)) + " " + std::to_string(*(&material->albedo.b));
	}
	else if(root + branch == "/:material.roughness"){
		bool success = true;
		float roughness = material->roughness;
		uint parm_count = 0;
		std::string value("");
		while(i < command.length()){
			if(command[i] == ' ') ++i;
			while(command[i] != ' '){
				if(i < command.length()){
					value += command[i];
					++i;
				}
				else break;
			}
			success = isFloatFormat(value);
			if(success){
				parm_count++;
			}
			else{
				material->roughness = roughness;
				break;
			}
		}
		if(parm_count == 1)
			material->roughness = std::stof(value);
		else if(success){
			strLog  = "Too many specified values!";
			success = false;
		}
		if(success)
			strLog = root + branch + " " + std::to_string(material->roughness);
	}
	else if(root + branch == "/:material.metallic"){
		bool success = true;
		float metallic = material->metallic;
		uint parm_count = 0;
		std::string value("");
		while(i < command.length()){
			if(command[i] == ' ') ++i;
			while(command[i] != ' '){
				if(i < command.length()){
					value += command[i];
					++i;
				}
				else break;
			}
			success = isFloatFormat(value);
			if(success){
				parm_count++;
			}
			else{
				material->metallic = metallic;
				break;
			}
		}
		if(parm_count == 1)
			material->metallic = std::stof(value);
		else if(success){
			strLog  = "Too many specified values!";
			success = false;
		}
		if(success)
			strLog = root + branch + " " + std::to_string(material->metallic);
	}
	else if(root + branch == "/:material.ao"){
		bool success = true;
		float ao = material->ao;
		uint parm_count = 0;
		std::string value("");
		while(i < command.length()){
			if(command[i] == ' ') ++i;
			while(command[i] != ' '){
				if(i < command.length()){
					value += command[i];
					++i;
				}
				else break;
			}
			success = isFloatFormat(value);
			if(success){
				parm_count++;
			}
			else{
				material->ao = ao;
				break;
			}
		}
		if(parm_count == 1)
			material->ao = std::stof(value);
		else if(success){
			strLog  = "Too many specified values!";
			success = false;
		}
		if(success)
			strLog = root + branch + " " + std::to_string(material->ao);
	}

}

void CommandLine::setParameter(std::string){
	//glClearColor
	//GL_MULTISAMPLER
	//glViewport
}

void CommandLine::render(std::string str, uint timestamp){
	if(!str.empty()){
		input->setPosition(2.0f, 20.0f);
		input->render(str + strInput);
	}
	if(!strLog.empty()){
		elapsedTime += 16;
		if(elapsedTime <= timestamp){
			input->setPosition(2.0f, 60.0f);
			input->render(strLog);
		}
		else{
			elapsedTime = 0.0f;
			strLog.clear();
		}
	}
}

void CommandLine::write(bool on){
	if(on)
		input->enableTextInput();
	else
		input->disableTextInput();
}