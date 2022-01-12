#ifndef TEXT_H
#define TEXT_H

#include "../SRW/SRW.hpp"

#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H 
#include <ftsizes.h>
#include <map>
#include <string>

class Glyph{
public:
	std::vector<unsigned char> buffer;
	uint texID;
	uint width;
	uint rows;
	uint bearingX;
	uint bearingY;
	int advance;
};

enum{
	STATIC, DYNAMIC, COUNTER
};

class Text : public SRW{
private:
	// - in use
	std::map<unsigned char, Glyph> glyphMap;
	uint VAO;
	uint VBO;
	uint texID;
	uint fontSize;
	uint pxSpacing;
	uint pxTab;
	std::string text;

	//- to refactor
	vec2<float> position;
	vec2<float> resolution;
	uint textureID;
	vec2<float> percentage;
	float xabs;
public:
	static mat4<float> canvasToClip;
	static std::vector<Text*> stack;
	
	Text(std::string path, uint px, float x, float y, vec3<float> color = vec3<float>(1.0f), std::string str = std::string(""), uint spacing = 2, uint tab = 50);
	Text(std::string path, uint px, float x, float y, vec3<float>color, uint type, std::string str = std::string(""));
	~Text();
	
	void free();
	void genBuffers();
	void write(std::string str);
	void updateCoordinates();
	void render();
	void render(std::string str);
	void enableTextInput();
	void disableTextInput();

	void keyboardEvent(SDL_Event* event, std::string& str);
	void windowEvent(SDL_Event* event);
	void setSpacing(uint px);
	void setPosition(float x, float y);
	
	static std::vector<std::string> read(std::string path);
	static void events(SDL_Event* event);
	static void renders();
	static void clean();
};
#endif