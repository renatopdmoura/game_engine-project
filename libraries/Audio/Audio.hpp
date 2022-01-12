#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_mixer.h>

void audio_test(){
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);
	if(init & flags != flags){
		std::cerr << "Function Mix_Init return with failed!" << std::endl;
	}
	else{
		if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024)){
			std::cerr << "Function Mix_OpenAudio return with failed!" << std::endl;
		}
		else{
			Mix_Chunk* sample;
			sample = Mix_LoadWAV("../assets/sfx/thunder.wav");
			if(!sample)
				std::cerr << "Function Mix_LoadWAV return with failed!" << std::endl;
			else if(Mix_PlayChannel(-1, sample, 0)){
				std::cerr << "Function Mix_PlayChannel return with failed!" << std::endl;
			}
		}
	}
	// Mix_CloseAudio();
}

#endif