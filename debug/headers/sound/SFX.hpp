#ifndef SFX_H
#define SFX_H
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

class SFX{
public:
	SFX(){
		mEffect = NULL;
		state = false;
	}
	Mix_Chunk* getEffect(){
		return mEffect;
	}
	void setEffect(std::string path){
		if(path != ""){
			mEffect = Mix_LoadWAV_RW(SDL_RWFromFile(path.c_str(), "rb"), 1);
			if(mEffect == NULL){
				SDL_Log("Failed to load effect sound! SDL_mixer Error: %s", Mix_GetError());
			}
		}
	}
	void freeResources(){
		Mix_FreeChunk(mEffect);
	}
	void onEffect(){
		state = true;
		if(mEffect != NULL)
			Mix_PlayChannel(-1, mEffect, 0);
	}
private:
 	Mix_Chunk* mEffect;
 	bool state;
};
#endif