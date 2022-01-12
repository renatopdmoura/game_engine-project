#include "Quad.hpp"

Quad::Quad(float x, float y, float w, float h){
	position.z   = x;
	position.w   = y;
	resolution.z = w;
	resolution.w = h; 
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glBindVertexArray(0);
	updateCoordinates();
	genShader("../shaders/new/vs_render_quad.glsl", "../shaders/new/fs_render_quad.glsl");
}

Quad::~Quad(){
	free();
}

void Quad::free(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Quad::updateCoordinates(){
	SDL_GetWindowSize(ext_window, &ext_screen_width, &ext_screen_height);	
	resolution.x = resolution.z * ext_screen_width / 100.0f;
	resolution.y = !resolution.w? resolution.x : ext_screen_height * resolution.w / 100.0f;
	position.x   = position.z * (ext_screen_width  - resolution.x) / 100.0f; 
	position.y   = position.w * (ext_screen_height - resolution.y) / 100.0f;
	float vertices[] = {
		position.x, position.y, 0.0f, 0.0f,
		position.x + resolution.x, position.y, 1.0f, 0.0f,
		position.x + resolution.x, position.y + resolution.y, 1.0f, 1.0f,
		position.x + resolution.x, position.y + resolution.y, 1.0f, 1.0f,
		position.x, position.y + resolution.y, 0.0f, 1.0f,
		position.x, position.y, 0.0f, 0.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::render(){
	glUseProgram(program);
	glBindVertexArray(VAO);
	initUnifSampler2D();
	initUnifMat4f();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Quad::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			updateCoordinates();
		}
	}
}