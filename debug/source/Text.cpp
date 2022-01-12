#include "../headers/ui/Text.hpp"

Text::Text(){

}

Text::Text(const char* path, uint size){
	genText(path, size);
}

void Text::genText(const char* path, uint size){
	this->fontSize = size;
	FT_Library ft;
	if(FT_Init_FreeType(&ft)){
		std::cout << "Error: Couldn't init freeType library!" << std::endl;
	}
	FT_Face face;
	if(FT_New_Face(ft, path, 0, &face)){
		std::cout << "Error: Failed to find font!" << std::endl;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	for(unsigned char c = 0; c < 128; ++c){
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
			std::cout << "Error: Failed to load glyph!" << std::endl;
			continue;
		}
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		Glyph glyph = {
						texture, 
						face->glyph->bitmap.width, face->glyph->bitmap.rows,
						face->glyph->bitmap_left, face->glyph->bitmap_top, 
						face->glyph->advance.x
					};
		glyphMap.insert(std::pair<char, Glyph>(c, glyph));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	createProgram(program);
	createShader(program, GL_VERTEX_SHADER,   readShaderSource("../shaders/text/vs_text.glsl"));
	createShader(program, GL_FRAGMENT_SHADER, readShaderSource("../shaders/text/fs_text.glsl"));
	linkProgram(program);
}

void Text::render(std::string text, float x, float y, float scale, vec3f color, mat4<float> orthoText){
	glUseProgram(program);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "fontTex"), 0);
	glUniform3fv(glGetUniformLocation(program, "colorText"), 1, color.address());
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float*>(orthoText.matrix));
	
	std::string::const_iterator c;
	for(c = text.begin(); c != text.end(); c++){
		Glyph glyph = glyphMap[*c];
		float xpos = x + glyph.bearingX * scale;
		float ypos = y - (glyph.height - glyph.bearingY) * scale;
		float w = glyph.width * scale;
		float h = glyph.height * scale;
		float vertices[6][4]{
			{ xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }  
		};
		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (glyph.advance >> 6) * scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Text::free(){
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}