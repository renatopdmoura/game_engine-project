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
public:
	// - Static functions
	static std::string uppercase(std::string str);
	static std::vector<std::string> read(std::string path);
	static void events(SDL_Event* event);

	// - Construtctors and desctructors
	Text();
	Text(std::string path, uint px, float x, float y, GUI_PanelLayout inside, uint spacing, uint tab);
	Text(std::string path, uint px, float x, float y, vec3<float> color = vec3<float>(1.0f), std::string str = std::string(""), GUI_PanelLayout inside = ON_WINDOW, uint spacing = 2, uint tab = 50);
	Text(std::string path, uint px, float x, float y, vec3<float>color, uint type, std::string str = std::string(""));
	~Text();
	void free();

	// - General functions
	void write(std::string str);
	void render();
	void render(std::string str);
	void enableTextInput();
	void disableTextInput();
	void picking();
	void keyboardEvent(SDL_Event* event, std::string& str);
	
	// - Setters
	void setSpacing(uint px);
	void setTextColor(vec3<float> color);
	
	// - Getters
	uint getTexID() const;
	std::string getLabel() const;
	vec3<float> getTextColor();
private:
	std::map<unsigned char, Glyph> glyphMap;
	uint texID;
	uint fontSize;
	uint pxSpacing;
	uint pxTab;
	std::string text;
	vec3<float> txtColor;
	std::string filepath;

	float marginLeft;
};
#endif