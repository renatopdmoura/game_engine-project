#ifndef MINIMAP_H
#define MINIMAP_H
#include <SDL.h>
#include "../extern/GLOBAL_VAR.hpp"
#include "../extern/COMMON.hpp"
#include "../SRW.hpp"

using namespace srw;

class Minimap : public SRW{
public:
	//Initialize
	Minimap(vec2uint res, vec2f init_size, vec2f resize, float abs_x, float abs_y, float rel_x, float rel_y, SDL_Scancode key = SDL_SCANCODE_UNKNOWN);
	void genBuffers();
	void genTexMinimap();
	
	//Rendering
	void render();
	void renderLocation(float angle, vec3f color);
	
	//Getters
	uint getFramebufferID() const;
	uint getWidth() const;
	uint getHeight() const;
	
	//Handle events
	void updateUI();
	void keyboardEvent(SDL_Event* event);
	void windowEvent(SDL_Event* event);
private:
	//Event key
	SDL_Scancode code;

	//Absolute, relative and auxiliary variables
	vec4f posAbsolute;
	vec4f posRelative;
	vec4f posTemp;
	vec2f wxhTemp;
	
	//Current resolution
	int scrWidth;
	int scrHeight;

	//Texture resolution, current size and max size UI 
	vec2uint resolution;
	uint  currWidth;
	uint  currHeight;
	uint  resizedWidth;
	uint  resizedHeight;

	//OpenGL objects
	uint  programArrow;
	uint  VAO[2];
	uint  VBO[2];
	uint  FBO;
	uint  RBO;
};
#endif