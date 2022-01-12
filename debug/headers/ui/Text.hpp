#ifndef TEXT_H
#define TEXT_H
#include "../extern/COMMON.hpp"
#include "../math/vec3.hpp"
#include "../math/mat4.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H 
#include <map>

class Text{
private:
	struct Glyph{
		uint textureID;
		uint width;
 		uint height;
		int bearingX;
		int bearingY;
		int advance;
	};
public:
	//Initialize and terminate
	Text();
	Text(const char* path, uint size);
	void genText(const char* path, uint size);
	void free();
	
	//Rendering
	void render(std::string text, float x, float y, float scale, vec3f color);

	//Getters
	uint getFontSize() const;
private:
	uint program;
	uint VAO;
	uint VBO;
	uint fontSize;
	std::map<char, Glyph> glyphMap;
};
#endif