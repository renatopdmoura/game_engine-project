#include "Panel.hpp"

Panel::Panel(float x, float y, float w, float h, float thickness, vec4<float>backgroundColor, vec4<float>headerColor, vec4<float>borderColor, GUI_PanelLayout panelLayout){
	setID(GUI::instancesCount++);
	setType(PANEL);
	setPanelLayout(panelLayout);
	setRatio(x, y, w, h);
	borderThickness    = thickness;
	rgbBackgroundColor = backgroundColor;
	rgbHeaderColor     = headerColor;
	rgbBorderColor     = borderColor;
	genBuffers();
	setProgram(SRW::programs[GUI_PROGRAM]);
	setUniform3f("ID", vec3<float>((float)ID / 256.0f));
	setUniform1f("fOffsetX", &offset.x);
	setUniform1f("fOffsetY", &offset.y);
	setUniform1f("fPosX", &position.x);
	setUniform1f("fPosY", &position.y);
	setUniform1f("fWidth", &resolution.w);
	setUniform1f("fHeight", &resolution.h);
	setUniform1f("fHeaderHeight", &headerHeight);
	setUniform1f("fBorderThickness", &borderThickness);				
	GUI::stack.push_back(this);
}

void Panel::picking(){
	glBindVertexArray(VAO);
	glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
	initUnif1f();
	initUnif3f();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	if(!childrens.empty()){
		for(std::vector<GUI*>::iterator it = childrens.begin(); it != childrens.end(); ++it)
			(*it)->picking();
	}
}

void Panel::render(){
	glUseProgram(SRW::programs[GUI_PANEL_PROG]);
	glBindVertexArray(VAO);
	if(!sampler2D.empty()){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sampler2D[0].texID);
		glUniform1i(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], sampler2D[0].label.c_str()), sampler2D[0].value);
		glUniform1i(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "flagTex"), 1);
	}
	else
		glUniform1i(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "flagTex"), 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
	glUniform4fv(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "rgbBorderColor"), 1, rgbBorderColor.address());
	glUniform4fv(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "rgbHeaderColor"), 1, rgbHeaderColor.address());
	glUniform4fv(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "rgbBackgroundColor"), 1, rgbBackgroundColor.address());
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fOffsetX"), offset.x);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fOffsetY"), offset.y);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fPosX"), position.x);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fPosY"), position.y);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fWidth"), resolution.w);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fHeight"), resolution.h);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fHeaderHeight"), headerHeight);
	glUniform1f(glGetUniformLocation(SRW::programs[GUI_PANEL_PROG], "fBorderThickness"), borderThickness);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
	if(!childrens.empty()){
		for(std::vector<GUI*>::iterator it = childrens.begin(); it != childrens.end(); ++it)
			(*(*it)).render();
	}
}