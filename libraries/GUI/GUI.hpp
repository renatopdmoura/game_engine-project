#ifndef GUI_H
#define GUI_H

#include "../SRW/SRW.hpp"

#include <functional>

enum GUI_Type{
	NON_SPECIFIC_TYPE,
	TEXT,
	PANEL,
	BUTTON
};
enum GUI_PanelLayout{
	ON_WINDOW,
	ON_HEADER,
	ON_BODY
}; 

class GUI : public SRW{
public:
	static std::vector<GUI*> stack;
	uint static uboGUI;
	uint static ubBinding;
	mat4<float> static canvasToClip;
	static unsigned char pixelColor;
	static unsigned char instancesCount;
	vec2<float> winSize;

	// - Constructor and destructor
	GUI();
	~GUI();
	void free();

	// - General functions
	virtual void picking() = 0;
	virtual void render()  = 0;
	virtual void updateIconBuffer() = 0; // - Reescrever para ser uma callback de evento
	void mouseEvent(SDL_Event* event);
	void windowEvent(SDL_Event* event);
	void updateBuffer();
	void push(GUI* guiObject);
	inline void searchBranches(std::vector<GUI*>& v, SDL_Event* event);
	void getOffsetPercentage();
	
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
	void setHeaderHeight(float height);
	void setHeaderColor(vec3<float> color);
	void setHidden(bool flag);
	void setUserEvent(std::function<void(void)> func){
		callback = func;
	}
	void setBorderColor(vec3<float> color);
	void setBorderThickness(float thickness);
	
	// - Getters
	unsigned char getID() const;
	GUI* getParent();
	vec4<float> getRatio();
	vec2<float> getPosition();
	vec2<float> getResolution();
	vec2<float> getOffset();
	GUI_Type getType();
	GUI_PanelLayout getPanelLayout();
	float getHeaderHeight() const;
	vec3<float> getHeaderColor() const;
	bool getHidden() const;
	float getBorderThickness() const;

	// - Necessário escrever getters e setters
	std::vector<GUI*> childrens;
protected:	
	void genBuffers();
	uint VAO;
	uint VBO;
	unsigned char ID;
	vec4<float> ratio;
	vec2<float> position;
	vec2<float> resolution;	
	vec2<float> offset;
	bool hidden;

	vec4<float> rgbHeaderColor;	
	vec4<float> rgbBorderColor;
	float headerHeight;
	float borderThickness;

	GUI* parent;
	GUI_Type type;
	GUI_PanelLayout layout;
	std::function<void(void)> callback;	
};

class Button : public GUI{
public:
	Button(float x, float y, float w, float h){
		type = BUTTON;
		setRatio(x, y, w, h);
		closeIcon();
		ID       = GUI::instancesCount++;
		GUI::stack.push_back(this);
	}

	void render(){
		glUseProgram(program);
		glBindVertexArray(iconVAO);
		glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
		glUniform1f(glGetUniformLocation(program, "fOffsetX"), offset.x);
		glUniform1f(glGetUniformLocation(program, "fOffsetY"), offset.y);
		glDrawArrays(GL_LINES, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void picking(){
		glBindVertexArray(VAO);
		glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
		glUniform3fv(glGetUniformLocation(SRW::programs[GUI_PROGRAM], "ID"), 1, vec3<float>(ID / 256.0f).address());
		glUniform1f(glGetUniformLocation(SRW::programs[GUI_PROGRAM], "fOffsetX"), offset.x);
		glUniform1f(glGetUniformLocation(SRW::programs[GUI_PROGRAM], "fOffsetY"), offset.y);
		glUniform1f(glGetUniformLocation(SRW::programs[GUI_PROGRAM], "fPosY"), position.y);
		glUniform1f(glGetUniformLocation(SRW::programs[GUI_PROGRAM], "fHeight"), resolution.h);
		glUniform1f(glGetUniformLocation(SRW::programs[GUI_PROGRAM], "fHeaderHeight"), resolution.h);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void closeIcon(){
		glGenVertexArrays(1, &iconVAO);
		glGenBuffers(1, &iconVBO);
		glBindVertexArray(iconVAO);
		glBindBuffer(GL_ARRAY_BUFFER, iconVBO);
		resolution.w = ratio.z * ext_screen_width / 100.0f;
		resolution.h = ratio.w == 0.0f? resolution.w: ratio.w * ext_screen_height / 100.0f;
		position.x   = ratio.x * (ext_screen_width - resolution.w) / 100.0f;
		position.y   = ratio.y * (ext_screen_height - resolution.h) / 100.0f;
		float vertices[4][4]{
					{ position.x,  position.y, 0.0f, 0.0f },            
		            { position.x + resolution.w, position.y + resolution.h, 0.0f, 1.0f },
		            { position.x, position.y + resolution.h,  1.0f, 1.0f },
		            { position.x + resolution.w,  position.y, 0.0f, 0.0f },
				};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0][0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4<float>), reinterpret_cast<void*>(0));
		glBindVertexArray(0);
		genShader("../shaders/ui/vs_gui.glsl", "../shaders/ui/fs_xico.glsl");
		
		// - Background
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		resolution.w = ratio.z * ext_screen_width / 100.0f;
		resolution.h = ratio.w == 0.0f? resolution.w: ratio.w * ext_screen_height / 100.0f;
		position.x   = ratio.x * (ext_screen_width - resolution.w) / 100.0f;
		position.y   = ratio.y * (ext_screen_height - resolution.h) / 100.0f;
		float quad[6][4]{
				{ position.x,  position.y + resolution.h, 0.0f, 0.0f },            
	            { position.x,  position.y, 0.0f, 1.0f },
	            { position.x + resolution.w, position.y,  1.0f, 1.0f },
	            { position.x,  position.y + resolution.h, 0.0f, 0.0f },
	            { position.x + resolution.w, position.y,  1.0f, 1.0f },
	            { position.x + resolution.w, position.y + resolution.h, 1.0f, 0.0f }  
			};
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad[0][0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4<float>), reinterpret_cast<void*>(0));
		glBindVertexArray(0);
	}

	void updateIconBuffer(){
		// resolution.w = ratio.z * parent->getResolution().w / 100.0f;
		// resolution.h = ratio.w == 0.0f? resolution.w: ratio.w * parent->getResolution().h / 100.0f;
		position.x = (ratio.x * (parent->getResolution().w - resolution.w) / 100.0f) + parent->getPosition().x;
		position.y = (ratio.y * (parent->getResolution().h - resolution.h) / 100.0f) + parent->getPosition().y;
		float quad[4][4]{
					{ position.x,  position.y, 0.0f, 0.0f },            
		            { position.x + resolution.w, position.y + resolution.h, 0.0f, 1.0f },
		            { position.x, position.y + resolution.h,  1.0f, 1.0f },
		            { position.x + resolution.w,  position.y, 0.0f, 0.0f },
				};
		glBindBuffer(GL_ARRAY_BUFFER, iconVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), &quad[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
private:
	uint iconVAO;
	uint iconVBO;
};


#endif