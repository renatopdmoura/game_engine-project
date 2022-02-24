#ifndef AUDIO_H
#define AUDIO_H

#include "../Common/Common.hpp"
#include "../Math/Math.hpp"
#include "../SRW/SRW.hpp"

#include <SDL.h>
#include <iostream>
#include <string>
#include <al.h>
#include <alc.h>
#include <vector>
#include <functional>

/*
	Carecteristicas:
	- musicStack: vetor de fábrica que armazena o endereço emissores que o membro 'format' é definido para 'AL_FORMART_STEREO16'
	- sfxStack: vetor de fábrica que armazena o endereço de emissores  que o membro 'format' é definido para 'AL_FORMAT_MONO16'
*/

enum Audio_Container_Type{
	BGM,
	SFX
};

class Emitter{
public:
	// - Methods
	Emitter(std::string parm_path, vec3<float> parm_pos, float parm_ref, float parm_range, float parm_rolloff, float parm_gain, int parm_format = AL_FORMAT_MONO16);
	void genBuffersGL();
	void render();

	// - Members
	std::string path;
	vec3<float> position;
	vec3<float> direction;
	float coneInnerAngle;
	float coneOuterAngle;
	float coneOuterGain;
	float reference;
	float range;
	float rolloff;
	float gain;
	float relgain;
	int format;
	float angle;
	uint program;
	uint VAO;
	uint VBO;
	uint IBO;
	std::function<void(void)> callback;
};

class Audio{
public:
	static std::vector<Audio*> bgmStack;
	static std::vector<Audio*> sfxStack;

	static void initialize(vec3<float>& position, vec3<float>& target, int distanceModel = AL_LINEAR_DISTANCE_CLAMPED);
	static void free();
	static void updateListener(vec3<float>& position, vec3<float>& target);
	static void listenerGain(float gain);
	static void playBGM();
	static void stopBGM();
	static void playSFX();
	static void stopSFX();
	static void playAll();
	static void stopAll();
	static void selectByDistance(vec3<float>& position);
	static void displayError(ALenum error, std::string callName = "non specified function");		
	static void contextInfo();

	Audio(Audio_Container_Type flag, std::vector<Emitter>& emitters);
	~Audio();

	void play();
	void stop();

	void setEmitters(std::vector<Emitter>& emitters);
	void selectSources(vec3<float>& position);
	void updateSource(uint index, vec3<float> vector);
	void renderSources();

	void buffersInfo();

	std::vector<uint>& getBuffers();
	std::vector<uint>& getSources();		
	std::vector<Emitter>& getEmitters();
private:
	static ALCdevice* device;
	static ALCcontext* context;
	std::vector<uint> buffers;
	std::vector<uint> sources;
	std::vector<Emitter> stackEmitters;
	Audio_Container_Type type;
};
#endif