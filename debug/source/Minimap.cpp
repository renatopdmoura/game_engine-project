#include "../headers/ui/Minimap.hpp"

Minimap::Minimap(vec2uint res, vec2f init_size, vec2f resize, float abs_x, float abs_y, float rel_x, float rel_y, SDL_Scancode key){
	genShader("../shaders/minimap/vs_minimap_ui.glsl", "../shaders/minimap/fs_minimap_ui.glsl");
	programArrow  = SRW::genProgramID("../shaders/minimap/vs_arrow.glsl", "../shaders/minimap/fs_arrow.glsl");
	resolution    = res;
	currWidth	  = init_size.x;
	currHeight	  = init_size.y;
	resizedWidth  = resize.x;
	resizedHeight = resize.y;
	SDL_GetWindowSize(window, &scrWidth, &scrHeight);
	posAbsolute.x = abs_x != 0.0f? ((float)(scrWidth  - 1) - currWidth)  / abs_x: 0.0f;
	posAbsolute.y = abs_y != 0.0f? ((float)(scrHeight - 1) - currHeight) / abs_y: 0.0f;
	posAbsolute.z = abs_x;
	posAbsolute.w = abs_y;
	posRelative.x = rel_x != 0.0f? ((float)(scrWidth  - 1) - resizedWidth)  / rel_x: 0.0f;
	posRelative.y = rel_y != 0.0f? ((float)(scrHeight - 1) - resizedHeight) / rel_y: 0.0f;
	posRelative.z = rel_x;
	posRelative.w = rel_y;
	code 		  = key;
	genTexMinimap();
	genBuffers();
}

void Minimap::genBuffers(){
	glGenVertexArrays(1, &VAO[0]);
	glBindVertexArray(VAO[0]);
	glGenBuffers(1, &VBO[0]);
	float vertices_buffer0[] = { posAbsolute.x, posAbsolute.y + currHeight, 0.0f, 1.0f, 
					   	 	   	 posAbsolute.x, posAbsolute.y, 0.0f, 0.0f,
					  	 	   	 posAbsolute.x + currWidth, posAbsolute.y,  1.0f, 0.0f,
					   	 	   	 posAbsolute.x + currWidth, posAbsolute.y,  1.0f, 0.0f,
					   	 	   	 posAbsolute.x + currWidth, posAbsolute.y + currHeight, 1.0f, 1.0f,
					   	 	   	 posAbsolute.x,  posAbsolute.y + currHeight, 0.0f, 1.0f
					 			};
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_buffer0), vertices_buffer0, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO[1]);
	glBindVertexArray(VAO[1]);
	glGenBuffers(1, &VBO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	float vertices_buffer1[] = { 0.0f,  1.0f, 0.5f, 1.0f,
							   	-1.0f, -1.0f, 0.0f, 0.0f,
						 	   	 1.0f, -1.0f, 1.0f, 0.0f
	 						};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_buffer1), vertices_buffer1, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glBindVertexArray(0);
}

void Minimap::updateUI(){
	float vertices[] = { posAbsolute.x, posAbsolute.y + currHeight, 0.0f, 1.0f, 
						 posAbsolute.x, posAbsolute.y, 0.0f, 0.0f,
						 posAbsolute.x + currWidth, posAbsolute.y,  1.0f, 0.0f,	  
						 posAbsolute.x + currWidth, posAbsolute.y,  1.0f, 0.0f,
						 posAbsolute.x + currWidth, posAbsolute.y + currHeight, 1.0f, 1.0f,
			   	 		 posAbsolute.x,  posAbsolute.y + currHeight, 0.0f, 1.0f
						};
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void Minimap::genTexMinimap(){
	uint tex_id;
	SRW::genFramebuffer(tex_id, RBO, FBO, resolution.w, resolution.h);
	addTexture(tex_id, "minimap", 0);
}

void Minimap::render(){
	SDL_GetWindowSize(window, &scrWidth, &scrHeight);
	scrWidth  -= 1;
	scrHeight -= 1;
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program);
	glBindVertexArray(VAO[0]);
	initUnifSampler2D();
	initUnifMat4fv();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Minimap::renderLocation(float angle, vec3f color){
	uint prog = program;
	setProgram(programArrow);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(programArrow);
	glBindVertexArray(VAO[1]);
	initUnifSampler2D();
	mat2<float> result = rotateZ_2D(-angle - 90.0f);
	glUniformMatrix2fv(glGetUniformLocation(programArrow, "rotate"), 1, GL_FALSE, &result.matrix[0][0]);
	glUniform3fv(glGetUniformLocation(programArrow, "color"), 1, color.address());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	setProgram(prog);
}

uint Minimap::getFramebufferID() const{
	return FBO;
}

uint Minimap::getWidth() const{
	return resolution.w;
}

uint Minimap::getHeight() const{
	return resolution.h;
}

void Minimap::keyboardEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN && event->key.repeat == 0){
		if(event->key.keysym.scancode == code){	
			wxhTemp    = vec2f(currWidth, currHeight);
			currWidth  = resizedWidth;
			currHeight = resizedHeight;
			posTemp       = posAbsolute;  
			posAbsolute.x = posRelative.z != 0.0f? ((float)scrWidth  - currWidth)  / posRelative.z: 0.0f;
		   	posAbsolute.y = posRelative.w != 0.0f? ((float)scrHeight - currHeight) / posRelative.w: 0.0f;
			updateUI();
		}
	}
	if(event->type == SDL_KEYUP && event->key.repeat == 0){
		if(event->key.keysym.scancode == code){
			currWidth   = wxhTemp.x;
			currHeight  = wxhTemp.y;
			posAbsolute = posTemp;
			posAbsolute.x = posAbsolute.z != 0.0f? ((float)scrWidth  - currWidth)  / posAbsolute.z: 0.0f;
		   	posAbsolute.y = posAbsolute.w != 0.0f? ((float)scrHeight - currHeight) / posAbsolute.w: 0.0f;
			updateUI();
		}
	}
}

void Minimap::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			posAbsolute.x = posAbsolute.z != 0.0f? (((float)event->window.data1 - 1) - currWidth)  / posAbsolute.z: 0.0f;
		   	posAbsolute.y = posAbsolute.w != 0.0f? (((float)event->window.data2 - 1) - currHeight) / posAbsolute.w: 0.0f;
			updateUI();
		}
	}
}

/*	COMENTÁRIOS:
	<Abaixo comentado um algoritmo que escala o objeto UI a partir das proporções de resolução de tela atuais>		

	float previousAspect = (float)srcWidth / (float)srcHeight;
	float currentAspect  = (float)event->window.data1 / (float)event->window.data2;
	if(currentAspect > previousAspect){
		currWidth  *= currentAspect;
		currHeight *= currentAspect;
		posAbsolute.x /= currentAspect;
		posAbsolute.y /= currentAspect;
	}
	else if(currentAspect < previousAspect){
		currWidth  /= previousAspect;
		currHeight /= previousAspect;
	}
*/