#include "../../headers/CORE.hpp"
#include "../../headers/SRW.hpp"
#include "../../headers/extern/COMMON.hpp"
#include "../../headers/math/Math.hpp"

int main(int argc, char** args){
	if(initialize(800, 600)){
		unsigned int VAO;
		unsigned int VBO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		float width  = 400;
		float height = 150;
		float position_x = (screenWidth - width) / 2;
		float position_y = (screenHeight - height) / 2;
		float thickness  = 12.0f;

		float quad[] = {
			  position_x, position_y,
			  position_x + width, position_y,
			  position_x + width, position_y + height,
			  position_x + width, position_y + height,
			  position_x, position_y + height,
			  position_x, position_y
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::string vs_shader = readShaderSource("shaders/vs.glsl");
		std::string fs_shader = readShaderSource("shaders/fs.glsl");
		unsigned int program = 0;
		createProgram(program);
		createShader(program, GL_VERTEX_SHADER,   vs_shader);
		createShader(program, GL_FRAGMENT_SHADER, fs_shader);
		linkProgram(program);

		float on    = 0.6f;
		float over  = 1.0f;
		float out   = 0.8f;
		float blend = 0.8f;
		float fillColor[3]   = {0.2f, 0.2f, 0.2f};
		float borderColor[3] = {0.4f};
		mat4<float> uiMatrix = ortho(screenWidth, 0.0f, screenHeight, 0.0f);

		bool quit = false;
		SDL_Event event;
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		while(!quit){
			while(SDL_PollEvent(&event) != 0){
				if(event.type == SDL_KEYDOWN){
					switch(event.key.keysym.scancode){
						case SDL_SCANCODE_ESCAPE:
							quit = true;
							break;
						default:
							break;
					}
				}
				if(event.type == SDL_MOUSEMOTION){
					bool inside_x = event.button.x > position_x && event.button.x < (position_x + width)? true: false;
					bool inside_y = event.button.y > position_y && event.button.y < (position_y + height)? true: false;
					if(inside_x && inside_y){
						blend = over;
					}
					else{
						blend = out;
					}
				}
			}
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(program);
			glBindVertexArray(VAO);
			glUniform1f(glGetUniformLocation(program, "x"), position_x);
			glUniform1f(glGetUniformLocation(program, "y"), position_y);
			glUniform1f(glGetUniformLocation(program, "w"), width);
			glUniform1f(glGetUniformLocation(program, "h"), height);
			glUniform3fv(glGetUniformLocation(program, "fillColor"), 1, fillColor);
			glUniform3fv(glGetUniformLocation(program, "borderColor"), 1, borderColor);
			glUniform1f(glGetUniformLocation(program, "blend"), blend);
			glUniform1f(glGetUniformLocation(program, "thickness"), thickness);
			glUniformMatrix4fv(glGetUniformLocation(program, "uiMatrix"), 1, GL_FALSE, &uiMatrix.matrix[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glUseProgram(0);

			SDL_GL_SwapWindow(window);
		}
		shutdown();
	}
	return 0;
}