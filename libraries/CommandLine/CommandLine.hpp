#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "../Common/Common.hpp"
#include "../Datatype/Datatype.hpp"
#include "../Text/Text.hpp"

#include <string>
#include <vector>

class CommandLine{
private:
	inline bool isFloatFormat(std::string& value);
public:
	CommandLine(Material* mtl, Text& in);
	void setMaterial(std::string command);
	void setParameter(std::string);
	void render(std::string str = std::string(""), uint timestamp = 1000);
	void write(bool on);
	Text* input;
	std::string strInput;
	std::string strLog;

private:
	Material* material;
	Sun* sun;
	float elapsedTime;
	vec2<float> posStrLog;
	vec2<float> posStrInput;
	vec2<float> colorStrLog;
	vec2<float> colorStrInput;
};

#endif