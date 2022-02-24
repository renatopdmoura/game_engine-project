#include "Checkbox.hpp"

Checkbox::Checkbox(float x, float y, GUI_PanelLayout panelLayout, float w, float h, vec4<float> marked, vec4<float> unmarked, float thickness, vec4<float> borderColor){
	setType(CHECKBOX);
	setPanelLayout(panelLayout);
	setID(GUI::instancesCount++);
	setOn(false);
	setBorderThickness(thickness);
	setRatio(x, y, w, h);
	rgbaMarkedColor   = marked;
	rgbaUnmarkedColor = unmarked;
	setBackgroundColor(rgbaUnmarkedColor);
	// setHeaderColor(unmarked);
	setBorderColor(vec4<float>(0.2f, 0.2f, 0.2f, 1.0f));
	genBuffers();
	setProgram(SRW::programs[GUI_PROGRAM]);
	setUniform3f("ID", vec3<float>((float)ID / 256.0f));
	setUniform1f("fOffsetX", &offset.x);
	setUniform1f("fOffsetY", &offset.y);
	setUniform1f("fPosX", &position.x);
	setUniform1f("fPosY", &position.y);
	setUniform1f("fWidth", &resolution.w);
	setUniform1f("fHeight", &resolution.h);
	setUniform1f("fHeaderHeight", &resolution.h);
	setUniform1f("fBorderThickness", &borderThickness);
	GUI::stack.push_back(this);
}

void Group::doUnique(uint active){
	if(checkbox.empty()) return;
	for(std::vector<Checkbox*>::iterator it = checkbox.begin(); it != checkbox.end(); ++it){
		if((*it)->getID() != active){
			(*it)->setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			(*it)->setOn(false);
		}
	}
}

Checkbox::~Checkbox(){

}

void Checkbox::render(){
	glUseProgram(SRW::programs[GUI_PANEL_PROG]);
	glBindVertexArray(VAO);
	glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
	glUniform4fv(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "rgbBorderColor"), 1, rgbBorderColor.address());
	glUniform4fv(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "rgbHeaderColor"), 1, rgbBackgroundColor.address());
	glUniform4fv(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "rgbBackgroundColor"), 1, rgbBackgroundColor.address());
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fOffsetX"), offset.x);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fOffsetY"), offset.y);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fPosX"), position.x);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fPosY"), position.y);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fWidth"), resolution.w);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fHeight"), resolution.h);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fHeaderHeight"), resolution.h);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fBorderThickness"), borderThickness);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Checkbox::picking(){
	glBindVertexArray(VAO);
	glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
	initUnif1f();
	initUnif3f();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Checkbox::setOn(bool flag){
	on = flag;
	if(on)
		rgbBackgroundColor = rgbaMarkedColor;
	else
		rgbBackgroundColor = rgbaUnmarkedColor;
}

bool Checkbox::getOn() const{
	return on;
}