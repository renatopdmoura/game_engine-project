#ifndef QUAD_H
#define QUAD_H

#include "../Common/Common.hpp"
#include "../SRW/SRW.hpp"

#include <SDL.h>

class Quad : public SRW{
public:
	Quad(float x, float y, float w, float h);
	~Quad();
	void free();
	void updateCoordinates();
	void windowEvent(SDL_Event* event);
	void render();
private:
	vec4<float> position;
	vec4<float> resolution;
	uint VAO;
	uint VBO;
};
#endif