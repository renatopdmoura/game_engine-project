#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "GUI.hpp"

class Checkbox : public GUI{
public:
	Checkbox(float x, float y,  GUI_PanelLayout panelLayout = ON_BODY, float w = 8.0f, float h = 0.0f, vec4<float> marked = vec4<float>(1.0f), vec4<float> unmarked = vec4<float>(0.6f, 0.6f, 0.6f), float thickness = 4.0f, vec4<float> borderColor = vec4<float>(0.2f, 0.2f, 0.2f));
	~Checkbox();
	void render();
	void picking();

	void setOn(bool flag);
	bool getOn() const;

private:
	bool on;
	vec4<float> rgbaMarkedColor;
	vec4<float> rgbaUnmarkedColor;
};

class Group{
public:
	std::vector<Checkbox*> checkbox;
	void doUnique(uint active = 0);
};

#endif