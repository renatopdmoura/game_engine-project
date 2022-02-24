#ifndef GUI_H
#define GUI_H

#include "../SRW/SRW.hpp"

#include <functional>

enum GUI_Type{
	NON_SPECIFIC_TYPE,
	TEXT,
	PANEL,
	CHECKBOX
};
enum GUI_PanelLayout{
	ON_WINDOW,
	ON_HEADER,
	ON_BODY
};

class GUI_Scale{
public:
	static std::vector<vec2<uint>> resolutions;
	std::vector<vec2<float>> percentages;
};

class GUI : public SRW{
public:
	// - Factory functions
	static std::vector<GUI*> stack;
	uint static uboGUI;
	uint static ubBinding;
	mat4<float> static canvasToClip;
	static unsigned char pixelColor;
	static unsigned char instancesCount;
	// - Constructor and destructor
	GUI();
	~GUI();
	void free();
	// - User functions
	virtual void picking() = 0;
	virtual void render()  = 0;
	void push(GUI* guiObject);	
	// - Factory functions
	void static guiUniformBuffer();
	void static stackPicking();
	void static stackRender();
	void static stackPollEvent(SDL_Event* event);	
	// - Setters
	void setID(unsigned char id);
	void setParent(GUI* guiObject);
	void setRatio(float x, float y, float w, float h);
	void setPosition(float x, float y);
	void setResolution(float w, float h);
	void setOffset(vec2<float> off);
	void setType(GUI_Type guiType);
	void setPanelLayout(GUI_PanelLayout distribute);
	void setResponsiveScale(std::vector<vec2<float>> values);
	void setBackgroundColor(vec4<float> color);
	void setHeaderHeight(float height);
	void setBorderThickness(float thickness);	
	void setHidden(bool flag);
	void setHeaderColor(vec4<float> color);
	void setBorderColor(vec4<float> color);
	void setUserEvent(std::function<void(void)> func);
	void setPin(bool flag);
	// - Getters
	uint getID() const;
	GUI* getParent();
	vec4<float> getRatio();
	vec2<float> getPosition();
	vec2<float> getResolution();
	vec2<float> getOffset();
	GUI_Type getType();
	GUI_PanelLayout getPanelLayout();
	float getHeaderHeight() const;
	float getBorderThickness() const;
	bool getHidden() const;
	vec4<float> getBackgroundColor();
	vec4<float> getHeaderColor();
	vec4<float> getBorderColor();
	bool getPin() const;
	// - Necessário escrever getters e setters
	std::vector<GUI*> childrens;
protected:	
	// - Internal functions
	void genBuffers();
	void updateBuffer();
	void mouseEvent(SDL_Event* event);
	void windowEvent(SDL_Event* event);
	void getOffsetPercentage();
	inline void searchBranches(std::vector<GUI*>& v, SDL_Event* event);
	// - OpenGL objects identifier
	uint VAO;
	uint VBO;
	// - To show
	unsigned char ID;
	vec4<float> ratio;
	vec2<float> position;
	vec2<float> resolution;	
	vec2<float> offset;
	bool hidden;
	bool pin;
	// - Formatting
	vec4<float> rgbBackgroundColor;
	vec4<float> rgbHeaderColor;	
	vec4<float> rgbBorderColor;
	float headerHeight;
	float borderThickness;
	vec2<float> winSize;
	// - To relationship
	GUI* parent;
	GUI_Type type;
	GUI_PanelLayout layout;
	GUI_Scale scale;
	std::function<void(void)> callback;	
};
#endif