#ifndef TEXT_H
#define TEXT_H

#include "../GUI/GUI.hpp"

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

class Text : public GUI{
private:
	// - in use
	std::map<unsigned char, Glyph> glyphMap;
	uint texID;
	uint fontSize;
	uint pxSpacing;
	uint pxTab;
	std::string text;

	uint textureID;
	float xabs;
public:
	static std::vector<Text*> stack;
	
	Text(std::string path, uint px, float x, float y, vec3<float> color = vec3<float>(1.0f), std::string str = std::string(""), GUI_PanelLayout inside = ON_WINDOW, uint spacing = 2, uint tab = 50);
	Text(std::string path, uint px, float x, float y, vec3<float>color, uint type, std::string str = std::string(""));
	~Text();

	void free();
	void write(std::string str);
	void render();
	void render(std::string str);
	void enableTextInput();
	void disableTextInput();
	void picking();

	void keyboardEvent(SDL_Event* event, std::string& str);
	void setSpacing(uint px);
	
	static std::vector<std::string> read(std::string path);
	static void events(SDL_Event* event);
	static void renders();
	static void clean();
};
#endif