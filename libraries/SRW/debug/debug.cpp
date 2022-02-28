#include "../../Common/Core.hpp"
#include "../SRW.hpp"

int main(int argv, char** args){
	if(Core::initialize("OpenGL", 800, 800)){
		SRW::genProgramShaders();
		SDL_Event event;
		while(!Core::Quit){
			while(SDL_PollEvent(&event) != 0){
				if(event.type == SDL_QUIT)
					Core::Quit = true;
			}	
			SDL_GL_SwapWindow(Core::Window);
		}	
	}
	
}