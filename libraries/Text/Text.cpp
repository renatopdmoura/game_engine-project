#include "Text.hpp"

std::vector<Text*> Text::stack;
mat4<float> Text::canvasToClip;

/*
	Teste 1
	Cada glifo salvo leva a largura abosuluta do glifo pela altura nominal(tamanho da fonte).
		> Não pode ser realizado ajuste na horizontal: crash por indexação inválida
	Teste 2
	Cada glifo salvo leva a largura absoluta do glifo pela altura nominal do glifo(tamanho da fonte).
	A linha com mais caracteres deve ser usada para determinar a largura horizontal da textura dado que o seu
	valor são incrementos da largura do glifo mais seu rolamento horizontal.
		> Tempo médio de cópia: 0.419s
		> Sem erros de indexação
		> Necessário resolver o rolamento vertical
*/

std::vector<std::string> Text::read(std::string path){
	std::ifstream file(path, std::ios_base::in);
	std::vector<std::string> strArray;
	std::string str;
	if(!file.is_open()){
		// - Log output
		// std::cerr << "Failed to find file \"" << path << "\"" << std::endl;
		for(std::string::iterator it = path.begin(); it != path.end(); ++it){
			if(*it == '\n'){
				strArray.push_back(str);
				str.clear();
			}
			else
				str += *it;
		}
		strArray.push_back(str);
		return strArray;
	}
	else{
		char c = '\0';
		while(file.get(c)){
			if(c == '\n'){
				strArray.push_back(str);
				str.clear();
			}
			else
				str += c;
		}
		strArray.push_back(str);
		return strArray;
	}
}

Text::Text(std::string path, uint px, float x, float y, vec3<float> color, std::string str, uint spacing, uint tab){
	// - Init memebers
	Text::stack.push_back(this);
	fontSize   = px < 12? 12: px;
	pxSpacing  = spacing;	
	pxTab      = tab;
	
	// - Init freetype
	FT_Library ft;
	if(FT_Init_FreeType(&ft)){
		std::cout << "Error: Couldn't init freeType library!" << std::endl;
	}
	FT_Face face;
	if(FT_New_Face(ft, path.c_str(), 0, &face)){
		std::cout << "Error: Failed to find font!" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	uint time_break = SDL_GetTicks() / 1000.0f;

	// - Load texture text at map
	std::map<unsigned char, Glyph> charMap;
	for(unsigned char c = 0; c < 128; ++c){
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
			std::cout << "Error: Failed to load glyph!" << std::endl;
			continue;
		}
		FT_Bitmap bmp = face->glyph->bitmap;
		Glyph glyph;
		glyph.width    = bmp.width;
		glyph.rows     = fontSize;
		glyph.bearingX = face->glyph->bitmap_left;
		glyph.bearingY = face->glyph->bitmap_top;
		glyph.advance  = (face->glyph->advance.x >> 6);
		glyph.buffer.resize(glyph.width * glyph.rows, 0);
		switch(uint(c)){
			case 32:
				glyph.width = fontSize / 6;
				glyph.buffer.resize(glyph.width * fontSize, 0);
				charMap.insert(std::pair<unsigned char, Glyph>(c, glyph));
				continue;
			case 9:
				glyph.width = pxTab;
				glyph.buffer.resize(glyph.width * fontSize, 0);
				charMap.insert(std::pair<unsigned char, Glyph>(c, glyph));
				continue;
		}
		for(uint j = 0; j < bmp.rows; ++j){
			for(uint i = 0; i < bmp.width; ++i){
				glyph.buffer[(j * glyph.width + i)] = bmp.buffer[j * bmp.width + i];
			}
		}
		charMap.insert(std::pair<unsigned char, Glyph>(c, glyph));
		// - Log output
		// std::cout << "char: " << (uint)c << " | " << c;
		// std::cout << " glyph.width: "    << glyph.width << " glyph.bearingX : " << glyph.bearingX;
		// std::cout << " glyph.rows: "     << bmp.rows    << " glyph.bearingY : " << glyph.bearingY << std::endl;
	}

	// - Get max characters per line and total of lines
	uint major_line = 0;
	std::vector<std::string> strArray = Text::read(str);
	for(uint i = 0; i < strArray.size(); ++i){
		major_line = i;
		for(uint j = 0; j < strArray.size(); ++j){
			if(strArray[major_line].length() < strArray[j].length())
				major_line = j;
		}
	}
	// - Log output
	// std::cout << "Total line: " << strArray.size() << "| Major line: " << major_line << "| Width: " << strArray[major_line].length() << std::endl;
	// for(uint i = 0; i < strArray.size(); ++i)
		// std::cout << strArray[i] << std::endl;

	// - Check maximum width
	uint index_major_line = 0;
	std::vector<uint> w_sizes;
	for(uint i = 0; i < strArray.size(); ++i){
		uint w = 0;
		for(uint j = 0; j < strArray[i].length(); ++j){
			w += charMap[strArray[i][j]].width + charMap[strArray[i][j]].bearingX + pxSpacing;
		}
		w_sizes.push_back(w);
		if(i == major_line) index_major_line = i;
	}
	// - Log output
	// std::cout << "max size:" << w_sizes[index_major_line] << " | " << fontSize * strArray.size() << std::endl;

	// - Prepare for reading OpenGL
	uint indice  = 0;
	uint offset  = 0;
	std::vector<unsigned char> glyphBuffer(w_sizes[index_major_line] * (fontSize * strArray.size()), 0);

	for(uint s = 0; s < strArray.size(); ++s){
		text = strArray[s];
		std::string::iterator it;
		for(uint rows = 0; rows < fontSize; ++rows){
			for(it = text.begin(); it != text.end(); ++it){
				indice += charMap[*it].bearingX + pxSpacing;
				offset = indice;
				for(uint columns = 0; columns < charMap[*it].width; ++columns){
					glyphBuffer[columns + offset] = charMap[*it].buffer[(rows * charMap[*it].width + columns)];
					indice++;
					// - Log output
					// std::cout << ((uint)glyphBuffer[columns + offset] > 0.1? 1: 0);
				}
				offset = indice;
			}
			// - Log output
			// std::cout << std::endl;
			indice += w_sizes[index_major_line] - w_sizes[s];
			offset  = indice;
		}
	}

	// - Log output
	// std::cout << (SDL_GetTicks() / 1000.0f) - time_break << "s" << std::endl;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w_sizes[index_major_line], fontSize * strArray.size(), 0, GL_RED, GL_UNSIGNED_BYTE, &glyphBuffer[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	genBuffers();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	float w    = w_sizes[index_major_line];
	float h    = fontSize * strArray.size();
	float xpos = x * (ext_screen_width  - w) / 100.0f;
	float ypos = y * (ext_screen_height - h) / 100.0f;
	float vertices[6][4]{
				{ xpos,     ypos + h,   0.0f, 0.0f },            
	            { xpos,     ypos,       0.0f, 1.0f },
	            { xpos + w, ypos,   	1.0f, 1.0f },
	            { xpos,     ypos + h,   0.0f, 0.0f },
	            { xpos + w, ypos,       1.0f, 1.0f },
	            { xpos + w, ypos + h,   1.0f, 0.0f }  
			};
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	genShader("../shaders/new/vs_render_quad.glsl", "../shaders/new/fs_text.glsl");
	setUniform3f("color", color);	
	setUniformMat4f("projection", &Text::canvasToClip);
	addTexture(texID, "tex", 0);
}

Text::Text(std::string path, uint px, float x, float y, vec3<float>color, uint type, std::string str){
	FT_Library ft;
	if(FT_Init_FreeType(&ft)){
		std::cout << "Error: Couldn't init freeType library!" << std::endl;
	}
	FT_Face face;
	if(FT_New_Face(ft, path.c_str(), 0, &face)){
		std::cout << "Error: Failed to find font!" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, px);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	switch(type){
		case DYNAMIC:
			for(unsigned char c = 0; c < 128; ++c){
				if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
					std::cout << "Error: Failed to load glyph!" << std::endl;
					continue;
				}		
				uint texID;
				glGenTextures(1, &texID);
				glBindTexture(GL_TEXTURE_2D, texID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				Glyph glyph;
				glyph.texID    = texID;
				glyph.width    = face->glyph->bitmap.width;
				glyph.rows     = face->glyph->bitmap.rows;
				glyph.bearingX = face->glyph->bitmap_left;
				glyph.bearingY = face->glyph->bitmap_top; 
				glyph.advance  = face->glyph->advance.x;
				glyphMap.insert(std::pair<unsigned char, Glyph>(c, glyph));
			}
			break;
		case COUNTER:
			str = "0123456789";	
			for(uint i = 0; i < str.length(); ++i){
				if(FT_Load_Char(face, str[i], FT_LOAD_RENDER)){
					std::cout << "Error: Failed to load glyph!" << std::endl;
					continue;
				}		
				uint texID;
				glGenTextures(1, &texID);
				glBindTexture(GL_TEXTURE_2D, texID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				Glyph glyph;
				glyph.texID    = texID;
				glyph.width    = face->glyph->bitmap.width;
				glyph.rows     = face->glyph->bitmap.rows;
				glyph.bearingX = face->glyph->bitmap_left;
				glyph.bearingY = face->glyph->bitmap_top; 
				glyph.advance  = face->glyph->advance.x;
				glyphMap.insert(std::pair<unsigned char, Glyph>(str[i], glyph));
			}
		break;
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	SDL_GetWindowSize(ext_window, &ext_screen_width, &ext_screen_height);
	position.x   = x;
	position.y   = y;
	xabs         = position.x;

	genBuffers();
	genShader("../shaders/new/vs_render_quad.glsl", "../shaders/new/fs_text.glsl");
	setUniform3f("color", color);	
	setUniformMat4f("projection", &Text::canvasToClip);
}

void Text::genBuffers(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Text::~Text(){
	free();
}
void Text::free(){
	glDeleteTextures(1, &textureID);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Text::write(std::string str){
}

void Text::updateCoordinates(){
}

void Text::render(std::string str){
	position.x = xabs;
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUnif3f();
	initUnifMat4f();
	std::string::iterator c;
	bool breakLine = false;
	uint countLine = 0;
	if(str.length() > 0) text = str;
	for(c = text.begin(); c != text.end(); ++c){
		if(*c == '\n'){
			breakLine = true;
			position.x = xabs;
			countLine++;
			continue;
		}
		else if(c == text.end())
			breakLine = false;
		Glyph glyph = glyphMap[*c];
		float xpos = position.x + glyph.bearingX;
		float ypos = (position.y - (breakLine? fontSize * countLine: 0) - (glyph.rows - glyph.bearingY));
		float w = glyph.width;
		float h = glyph.rows;
		float vertices[6][4]{
				{ xpos,     ypos + h,   0.0f, 0.0f },            
	            { xpos,     ypos,       0.0f, 1.0f },
	            { xpos + w, ypos,   	1.0f, 1.0f },
	            { xpos,     ypos + h,   0.0f, 0.0f },
	            { xpos + w, ypos,       1.0f, 1.0f },
	            { xpos + w, ypos + h,   1.0f, 0.0f }  
			};
		glBindTexture(GL_TEXTURE_2D, glyph.texID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		position.x += (uint)*c == 9? Text::pxTab: (glyph.advance >> 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);	
}

void Text::render(){
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUnifSampler2D();
	initUnif3f();
	initUnifMat4f();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);	
}

void Text::enableTextInput(){
	SDL_StartTextInput();
}

void Text::disableTextInput(){
	SDL_StopTextInput();
}

void Text::keyboardEvent(SDL_Event* event, std::string& str){
	switch(event->type){
		case SDL_TEXTINPUT:
			str += event->text.text;
			break;
		case SDL_KEYDOWN:
			switch(event->key.keysym.scancode){
				case SDL_SCANCODE_BACKSPACE:
					if(str.length() > 0){
						str.pop_back();
					}
				break;
			}
			break;
	}
}

void Text::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			updateCoordinates();
		}
	}	
}

void Text::events(SDL_Event* event){
	for(uint i = 0; i < stack.size(); ++i){
		stack[i]->windowEvent(event);
	}
}


void Text::setSpacing(uint px){
	pxSpacing = px;
}

void Text::setPosition(float x, float y){
	position = vec2<float>(x, y);
}

void Text::renders(){
	for(uint i = 0; i < stack.size(); ++i){
		stack[i]->render();
	}
}

void Text::clean(){
	for(uint i = 0; i < stack.size(); ++i){
		stack[i]->free();
	}
}