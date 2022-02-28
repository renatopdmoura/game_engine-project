#include "GL/glew.h"
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include <iostream>

int main(int argv, char** args){
    SDL_Window* window = NULL;
	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |  SDL_WINDOW_SHOWN;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
		SDL_Log("SDL unable initialize! SDL Error: %s", SDL_GetError());
	}
	else{
		//Define a versão principal e secundária do núcleo OpenGL 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);	
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		
		//Define o tamanho de determinados buffers
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		//Define o perfil OpenGL, neste caso, desabilita a versão legada
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		
		//Cria uma janela OpenGL
		window = SDL_CreateWindow("MSVC Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, flags);

		if(window == NULL){
			SDL_Log("Couldn't create window! SDL Error: %s", SDL_GetError());
		}
		else{
			SDL_SetWindowBordered(window, SDL_bool(true));
			SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
			SDL_SetCursor(cursor);
			SDL_ShowCursor(SDL_TRUE);
			//Define o contexto OpenGL
			SDL_GLContext context = SDL_GL_CreateContext(window);
			if(context == NULL)
				SDL_Log("Couldn't create context! SDL Error: %s", SDL_GetError());
			else{
				//Carrega extensões OpenGL ARB, EXT, e.g.
				glewExperimental = GL_TRUE;
				GLenum error = glewInit();
				if(error != GLEW_OK){
					SDL_Log("glewInit failed! %s", glewGetErrorString(error));
				}
				else{
				    //Configura a taxa de troca de quadros
				    if(SDL_GL_SetSwapInterval(-1) < 0){
					    if(SDL_GL_SetSwapInterval(1) < 0)
						    SDL_Log("Warning: Unable to set Vsync! SDL Error: %s", SDL_GetError());
					}
				}
			}
		}
	}
    return 0;
}