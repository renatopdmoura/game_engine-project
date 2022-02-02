#ifndef PANEL_H
#define PANEL_H

#include "GUI.hpp"

class Panel : public GUI{
public:
	Panel(float x, float y, float w, float h, float thickness, vec4<float>backgroundColor, vec3<float>headerColor, vec3<float>borderColor = vec3<float>(0.2f), GUI_PanelLayout panelLayout = ON_WINDOW);
	void render();
	void picking();
	void updateIconBuffer(){}

	// - Setters
	void setBackgroundColor(vec3<float> color);
private:
	vec4<float> rgbBackgroundColor;
	
};
#endif