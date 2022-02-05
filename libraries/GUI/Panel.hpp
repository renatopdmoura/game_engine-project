#ifndef PANEL_H
#define PANEL_H

#include "GUI.hpp"

class Panel : public GUI{
public:
	Panel(float x, float y, float w, float h, float thickness, vec4<float>backgroundColor, vec4<float>headerColor, vec4<float>borderColor = vec4<float>(1.0f), GUI_PanelLayout panelLayout = ON_WINDOW);
	void render();
	void picking();
};
#endif