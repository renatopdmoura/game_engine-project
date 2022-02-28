#include "../../Common/Core.hpp"
#include "../../SRW/SRW.hpp"
#include "../Camera.hpp"

int main(int argv, char** args){
	if(Core::initialize("OpenGL", 800, 800)){
		Camera fpsCam(vec3<float>(0.0f, 0.0f, 0.0f), vec3<float>(0.0f, 0.0f, -1.0f), 45.0f, 0.1f, 20.0f);
		fpsCam.getViewAndProjectionMatrix().show("viewProj");
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