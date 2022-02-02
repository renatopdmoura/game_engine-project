#include "GUI.hpp"

std::vector<GUI*> GUI::stack;
uint GUI::uboGUI    	          = 0;
uint GUI::ubBinding               = 5;
mat4<float> GUI::canvasToClip     = orthographic(0.0f, (float)ext_screen_width, 0.0f, (float)ext_screen_height);
unsigned char GUI::instancesCount = 1;
unsigned char GUI::pixelColor     = 0;

GUI::GUI(){
	VAO    		 	= 0;
	VBO    		 	= 0;
	ID     		 	= 0;
	type   		 	= NON_SPECIFIC_TYPE;
	layout 		 	= ON_WINDOW;
	parent 		    = NULL;
	headerHeight    = 40.0f;
	borderThickness = 0.0f;
	hidden 	        = false;
	winSize 	    = vec2<float>((float)ext_screen_width, (float)ext_screen_height);
	callback        = [&](){ /*std::cout << "Object ID " << (uint)ID << std::endl;*/};
}

GUI::~GUI(){
	free();
}

void GUI::free(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	parent = NULL;
	childrens.clear();
}

// - General functions

void GUI::genBuffers(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4<float>) * 6, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4<float>), reinterpret_cast<void*>(0));
	glBindVertexArray(0);
	updateBuffer();	
}

void GUI::mouseEvent(SDL_Event* event){
	switch(event->type){
		case SDL_MOUSEBUTTONDOWN:
			if(event->button.button == SDL_BUTTON_LEFT){
				glReadPixels(event->button.x, ext_screen_height - event->button.y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &GUI::pixelColor);
				if(GUI::pixelColor == ID && parent == NULL){
					std::vector<GUI*> nodes(GUI::stack.size());
					for(uint i = 0; i < nodes.size(); ++i){
						nodes[i] = GUI::stack[i];
					}
					int j = nodes.size();
					for(int i = (GUI::stack.size() - 2); i >= 0; --i){
						j--;
						if(nodes[j]->getID() == ID) j = i;
						GUI::stack[i] = nodes[j];
					}
					GUI::stack.back() = this;
				}
				else if(GUI::pixelColor == ID){
					callback();
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			SDL_SetWindowGrab(ext_window, SDL_FALSE);
			GUI::pixelColor = 0;
			break;
		case SDL_MOUSEMOTION:
			if(GUI::pixelColor == ID && parent == NULL){
				SDL_SetWindowGrab(ext_window, SDL_TRUE);
				offset.x += (float)event->motion.xrel;
				offset.y -= (float)event->motion.yrel;
				if(position.x + offset.x < 0.0f || position.x + resolution.w + offset.x > ext_screen_width)
					offset.x -= (float)event->motion.xrel;
				if(position.y + offset.y < 0.0f || position.y + resolution.h + offset.y > ext_screen_height)
					offset.y += (float)event->motion.yrel;
				searchBranches(childrens, event);
			}
			break;
	}
}

void GUI::windowEvent(SDL_Event* event){
	switch(event->type){
		case SDL_WINDOWEVENT:
			if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				updateBuffer();
				searchBranches(childrens, event);				
			}
		break;
	}
}

void GUI::updateBuffer(){
	// - Determinar se o objeto em questão é um nó
	bool isNode = parent != NULL? true: false;
	
	// - As coordenadas do objeto dependem da janela de visualização
	if(layout == ON_WINDOW){
		if(type == PANEL){
			resolution.w = ratio.z * (float)ext_screen_width / 100.0f;
			resolution.h = ratio.w == 0.0f? resolution.w: ratio.w * (float)ext_screen_height / 100.0f;
		}		
		position.x = ratio.x * ((float)ext_screen_width  - resolution.w) / 100.0f;
		position.y = ratio.y * ((float)ext_screen_height - resolution.h) / 100.0f;
	}
	// - As coordenadas do objeto se ajustam ao cabeçalho do objeto pai
	else if(layout == ON_HEADER && isNode){
		position.x = (ratio.x * ((parent->getResolution().w - parent->getBorderThickness() * 2) - resolution.w) / 100.0f) + parent->getPosition().x;
		position.y = (ratio.y * (parent->getHeaderHeight() - resolution.h) / 100.0f) + parent->getPosition().y + parent->getBorderThickness() + parent->getResolution().h - (parent->getHeaderHeight() + parent->getBorderThickness() * 2.0f);
	}
	// - As coordenadas do objeto se ajustam ao corpo do objeto pai
	else if(layout == ON_BODY && isNode){
		if(type == PANEL){
			resolution.w = ratio.z * (parent->getResolution().w - parent->getBorderThickness() * 2) / 100.0f;	
			resolution.h = ratio.w != 0.0f? ratio.w * (parent->getResolution().h - (parent->getHeaderHeight() + parent->getBorderThickness() * 2.0f)) / 100.0f: resolution.w;
		}
		position.x = (ratio.x * ((parent->getResolution().w - parent->getBorderThickness() * 2) - resolution.w) / 100.0f) + parent->getPosition().x;
		position.y = (ratio.y * (parent->getResolution().h - (parent->getHeaderHeight() + parent->getBorderThickness() * 2.0f) - resolution.h) / 100.0f) + parent->getPosition().y;
		position.x += parent->getBorderThickness();
		position.y += parent->getBorderThickness();
	}
	// - Atualiza o quadrante de renderização
	float quad[6][4]{
				{ position.x,  position.y + resolution.h, 0.0f, 0.0f },            
	            { position.x,  position.y, 0.0f, 1.0f },
	            { position.x + resolution.w, position.y,  1.0f, 1.0f },
	            { position.x,  position.y + resolution.h, 0.0f, 0.0f },
	            { position.x + resolution.w, position.y,  1.0f, 1.0f },
	            { position.x + resolution.w, position.y + resolution.h, 1.0f, 0.0f }  
			};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), &quad[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	getOffsetPercentage();
}

void GUI::push(GUI* guiObject){
	for(std::vector<GUI*>::iterator it = stack.begin(); it != stack.end(); ++it){
		if((*(*it)).getID() == guiObject->getID()){
			stack.erase(it);
			break;
		}
	}
	guiObject->parent = this;
	guiObject->updateBuffer();
	childrens.push_back(guiObject);
}

void GUI::searchBranches(std::vector<GUI*>& v, SDL_Event* event){
	for(std::vector<GUI*>::iterator i = v.begin(); i != v.end(); ++i){
		(*(*i)).mouseEvent(event);
		if(event->type == SDL_MOUSEMOTION){
			float x = (*(*i)).getParent()->getOffset().x;
			float y = (*(*i)).getParent()->getOffset().y;
			(*(*i)).setOffset(vec2<float>(x, y));
		}
		(*(*i)).windowEvent(event);
		if(!(*(*i)).childrens.empty()){
			(*(*i)).searchBranches((*(*i)).childrens, event);
		}
	}
}

void GUI::getOffsetPercentage(){
	float scaleX = offset.x * 100.0f / winSize.w;
	float scaleY = offset.y * 100.0f / winSize.h;
	offset.x = scaleX * ext_screen_width / 100.0f;
	offset.y = scaleY * ext_screen_height / 100.0f;
	winSize = vec2<float>((float)ext_screen_width, (float)ext_screen_height);
}

// - Factory functions

void GUI::stackPicking(){
	glViewport(0, 0, ext_screen_width, ext_screen_height);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(SRW::programs[GUI_PROGRAM]);
	for(std::vector<GUI*>::iterator it = GUI::stack.begin(); it != GUI::stack.end(); ++it){
		if(*it == NULL) return;
		if(!(*(*it)).hidden)
			(*(*it)).picking();
	}
	glUseProgram(0);
}

void GUI::stackRender(){
	std::vector<GUI*>::iterator it;
	for(it = GUI::stack.begin(); it != GUI::stack.end(); ++it){
		if(*it == NULL) return;
		if(!(*(*it)).hidden)
			(*(*it)).render();
	}
}

void GUI::stackPollEvent(SDL_Event* event){
	for(std::vector<GUI*>::iterator it = GUI::stack.begin(); it != GUI::stack.end(); ++it){
		if(*it == NULL) return;
		switch(event->type){
			case SDL_WINDOWEVENT:
				if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					GUI::guiUniformBuffer();
				break;
		}
		if(!(*(*it)).hidden){
			(*(*it)).mouseEvent(event);
			(*(*it)).windowEvent(event);
			(*(*it)).searchBranches((*(*it)).childrens, event);
		}
	}
}

void GUI::guiUniformBuffer(){
	SDL_GetWindowSize(ext_window, &ext_screen_width, &ext_screen_height);
	GUI::canvasToClip = orthographic(0.0f, (float)ext_screen_width, 0.0f, (float)ext_screen_height);
	uint prog = SRW::programs[GUI_PROGRAM];
	const char* ubName   = "GUI";
	uint ubNumberMembers       = 1;
	const char* ubMemberName[] = {"canvasToClip"};
	uint ubMemberIndex 		   = 0;
	
	if(glIsBuffer(GUI::uboGUI))
		glDeleteBuffers(1, &GUI::uboGUI);

	uint ubIndex = glGetUniformBlockIndex(prog, ubName);
	if(ubIndex == GL_INVALID_INDEX){
		std::cerr << "GUI::ERROR::Uniform block name is not a have a valid index!" << std::endl;
	}else{
		int ubSize = -1;
		glGetActiveUniformBlockiv(prog, ubIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &ubSize);
		if(ubSize == -1)
			std::cerr << "GUI::ERROR::An error occurred getting the minimum size for the buffer!" << std::endl;
		else{
			unsigned char* buffer = new unsigned char[ubSize];
			glGetUniformIndices(prog, ubNumberMembers, ubMemberName, &ubMemberIndex);
			if(ubMemberIndex == GL_INVALID_INDEX){
				std::cerr << "GUI::ERROR::" << ubMemberName << " is not a program variable shader or is not active!" << std::endl;
			}
			else{
				std::vector<int> offset(1);
				glGetActiveUniformsiv(prog, ubNumberMembers, &ubMemberIndex , GL_UNIFORM_OFFSET, offset.data());
				std::memcpy(buffer, &GUI::canvasToClip[0][0], sizeof(mat4<float>));
				glGenBuffers(1, &GUI::uboGUI);
				glBindBufferBase(GL_UNIFORM_BUFFER, GUI::ubBinding, GUI::uboGUI);
				glBufferData(GL_UNIFORM_BUFFER, ubSize, buffer, GL_DYNAMIC_DRAW);
				if(false){
					float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY));
					if(ptr != NULL){
						vec4<float>(ptr[0], ptr[1], ptr[2], ptr[3]).show("canvasToClip");
						vec4<float>(ptr[4], ptr[5], ptr[6], ptr[7]).show("canvasToClip");
						vec4<float>(ptr[8], ptr[9], ptr[10], ptr[11]).show("canvasToClip");
						vec4<float>(ptr[12], ptr[13], ptr[14], ptr[15]).show("canvasToClip");
					}
				}
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			}
			delete[] buffer;
		}
	}
}

// - Setters

void GUI::setParent(GUI* guiObject){
	parent = guiObject;
}

void GUI::setID(unsigned char id){
	ID = id;
}

void GUI::setHeaderHeight(float height){
	headerHeight = height;
}

void GUI::setRatio(float x, float y, float w, float h){
	ratio = vec4<float>(x, y, w, h);
}

void GUI::setPosition(float x, float y){
	position = vec2<float>(x, y);
}

void GUI::setResolution(float w, float h){
	resolution = vec2<float>(w, h);
}

void GUI::setOffset(vec2<float> off){
	offset = off;
}

void GUI::setType(GUI_Type guiType){
	type = guiType;
}

void GUI::setPanelLayout(GUI_PanelLayout distribute){
	layout = distribute;
}

void GUI::setHidden(bool flag){
	hidden = flag;
}

void GUI::setBorderColor(vec3<float> color){
	rgbBorderColor = color;
}

void GUI::setBorderThickness(float thickness){
	borderThickness = thickness;
	if(borderThickness > 0.0f){
		float n = borderThickness * 100.0f / resolution.w;
		float m = borderThickness * 100.0f / resolution.h;
		n = n > 100.0f? 100.0f: n;
		m = m > 100.0f? 100.0f: m;

		ratio.z = n == 100.0f? 100.0f: ratio.z + n;
		ratio.w = ratio.w != 0.0f? m == 100.0f? 100.0f: ratio.w + m: 0.0f;
		updateBuffer();
	}
}

// - Getters

GUI* GUI::getParent(){
	return parent;
}

unsigned char GUI::getID() const{
	return ID;
}

float GUI::getHeaderHeight() const{
	return headerHeight;
}

vec4<float> GUI::getRatio(){
	return ratio;
}

vec2<float> GUI::getPosition(){
	return position;
}

vec2<float> GUI::getResolution(){
	return resolution;
}

vec2<float> GUI::getOffset(){
	return offset;
}

GUI_Type GUI::getType(){
	return type;
}

GUI_PanelLayout GUI::getPanelLayout(){
	return layout;
}

float GUI::getBorderThickness() const{
		return borderThickness;
}

bool GUI::getHidden() const{
	return hidden;
}