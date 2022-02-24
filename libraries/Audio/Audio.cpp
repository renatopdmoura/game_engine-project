#include "Audio.hpp"

ALCdevice* Audio::device   = NULL;
ALCcontext* Audio::context = NULL;
std::vector<Audio*> Audio::bgmStack;
std::vector<Audio*> Audio::sfxStack;

Emitter::Emitter(std::string parm_path, vec3<float> parm_pos, float parm_ref, float parm_range, float parm_rolloff, float parm_gain, int parm_format){
	path 	       = parm_path;
	position       = parm_pos;
	coneInnerAngle = 0.0f;
	coneOuterAngle = 0.0f;
	coneOuterGain  = 0.0f;
	reference 	   = parm_ref;
	range 	       = parm_range;
	rolloff        = parm_rolloff;
	gain 	       = parm_gain;
	format 	       = parm_format;
	direction      = vec3<float>(0.0f);
	angle          = 0.0f;
	program        = SRW::genProgramShader("../shaders/audio/vs_cone_range.glsl", "../shaders/audio/fs_cone_range.glsl");
	if(format == AL_FORMAT_STEREO16) direction = vec3<float>(0.0f, 1.0f, 0.0f);
	genBuffersGL();
}

void Emitter::genBuffersGL(){
	std::vector<vec3<float>> vertices;
	for(float angle = 0.0f; angle <= 360.0f; angle += 15.0f){
		float x = cosf(radians(angle));
		float z = sinf(radians(angle));
		vertices.push_back(vec3<float>(x, 0.0f, z));
	}
	vertices.push_back(vec3<float>(0.0f, 0.0f, 0.0f));

	uint c = vertices.size() - 1;
	std::vector<uint> indices = {
		0, 1, 1, 2, 2, 3, 3, 4,	4, 5, 6, 7,	7, 8, 8, 9,	9, 10, 11, 12, 12, 13,
		13, 14,	14, 15,	15, 16,	16, 17, 17, 18,	18, 19,	20, 21,	21, 22,	22, 23
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3<float>) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
	glBindVertexArray(0);
}

void Emitter::render(){
	angle++;
	glUseProgram(program);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &(scale(vec3<float>(reference, 1.0f, reference)) * rotateY(angle) * translate(position))[0][0]);
	glDrawElements(GL_LINE_LOOP, 40, GL_UNSIGNED_INT, NULL);
	glDrawElements(GL_POINT, 40, GL_UNSIGNED_INT, NULL);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &(scale(vec3<float>(range, 1.0f, range)) * rotateY(-angle) * translate(position))[0][0]);
	glDrawElements(GL_LINE_LOOP, 40, GL_UNSIGNED_INT, NULL);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &(scale(vec3<float>(range, 1.0f, range)) * rotateX(90.0f) * rotateY(-angle) * translate(position))[0][0]);
	glDrawElements(GL_LINE_LOOP, 40, GL_UNSIGNED_INT, NULL);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &(scale(vec3<float>(range, 1.0f, range)) * rotateZ(90.0f) * rotateY(-angle) * translate(position))[0][0]);
	glDrawElements(GL_LINE_LOOP, 40, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Audio::initialize(vec3<float>& position, vec3<float>& target, int distanceModel){
	Audio::device = alcOpenDevice(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	if(Audio::device == NULL)
		std::cout << "Unable find device!" << std::endl;
	else{
		Audio::context = alcCreateContext(Audio::device, NULL);
		if(!alcMakeContextCurrent(Audio::context)){
			std::cout << "Unable to create context!" << std::endl;
			exit(1);
		}
		alDistanceModel(distanceModel);
		displayError(alGetError(), "alDistanceModel");
		alDopplerFactor(1.0f);					
		vec3<float> forward = position + target;
		ALfloat orientation[] = {forward.x, forward.y, forward.z, 0.0f, 1.0f, 0.0f};
		alListenerfv(AL_POSITION, position.address());
		displayError(alGetError(), "alListener3f<ALenum AL_POSITION>");
		alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		displayError(alGetError(), "alListener3f<ALenum AL_VELOCITY>");
		alListenerfv(AL_ORIENTATION, orientation);
		displayError(alGetError(), "alListenerfv<ALenum AL_ORIENTATION>");
		alListenerf(AL_GAIN, 1.0f);
		displayError(alGetError(), "alListenerf<ALenum AL_GAIN>");
		// Audio::contextInfo();
	}
}

void Audio::free(){
	bgmStack.clear();
	sfxStack.clear();
	Audio::context = alcGetCurrentContext();
	Audio::device  = alcGetContextsDevice(Audio::context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Audio::context);
	alcCloseDevice(Audio::device);
}

void Audio::updateListener(vec3<float>& position, vec3<float>& target){
	// - Check current context
	if(!alcMakeContextCurrent(Audio::context)){
		std::cout << "Unable to create context!" << std::endl;
		exit(1);
	}

	// - Update listener coordinates
	ALfloat orientation[] = {target.x, target.y, target.z, 0.0f, 1.0f, 0.0f};
	alListenerfv(AL_POSITION, position.address());
	displayError(alGetError(), "alListener3f<ALenum AL_POSITION>");
	alListenerfv(AL_ORIENTATION, orientation);
	displayError(alGetError(), "alListenerfv<ALenum AL_ORIENTATION>");
}

void Audio::playBGM(){
	for(std::vector<Audio*>::iterator it = bgmStack.begin(); it != bgmStack.end(); ++it)
		(*it)->play();
}

void Audio::stopBGM(){
	for(std::vector<Audio*>::iterator it = bgmStack.begin(); it != bgmStack.end(); ++it)
		(*it)->stop();
}

void Audio::playSFX(){
	for(std::vector<Audio*>::iterator it = sfxStack.begin(); it != sfxStack.end(); ++it)
		(*it)->play();
}

void Audio::stopSFX(){
	for(std::vector<Audio*>::iterator it = sfxStack.begin(); it != sfxStack.end(); ++it)
		(*it)->stop();
}

void Audio::playAll(){
	for(std::vector<Audio*>::iterator it = bgmStack.begin(); it != bgmStack.end(); ++it)
		(*it)->play();
	for(std::vector<Audio*>::iterator it = sfxStack.begin(); it != sfxStack.end(); ++it)
		(*it)->play();
}

void Audio::stopAll(){
	for(std::vector<Audio*>::iterator it = bgmStack.begin(); it != bgmStack.end(); ++it)
		(*it)->stop();
	for(std::vector<Audio*>::iterator it = sfxStack.begin(); it != sfxStack.end(); ++it)
		(*it)->stop();
}

void Audio::selectByDistance(vec3<float>& position){
	for(std::vector<Audio*>::iterator it = bgmStack.begin(); it != bgmStack.end(); ++it)
		(*it)->selectSources(position);		
	for(std::vector<Audio*>::iterator it = sfxStack.begin(); it != sfxStack.end(); ++it)
		(*it)->selectSources(position);
}

void Audio::displayError(ALenum error, std::string callName){
	if(error != AL_NO_ERROR){
	switch(error){
		case AL_INVALID_NAME:
			std::cout << "OpenAL error at " << callName << ": AL_INVALID_NAME" << std::endl;
			break;
		case AL_INVALID_ENUM:
			std::cout << "OpenAL error at " << callName << ": AL_INVALID_ENUM" << std::endl;
			break;				
		case AL_INVALID_VALUE:
			std::cout << "OpenAL error at " << callName << ": AL_INVALID_VALUE" << std::endl;
			break;
		case AL_INVALID_OPERATION:
			std::cout << "OpenAL error at " << callName << ": AL_INVALID_OPERATION" << std::endl;
			break;
		case AL_OUT_OF_MEMORY:
			std::cout << "OpenAL error at " << callName << ": AL_OUT_OF_MEMORY" << std::endl;
			break;
		}
		Audio::free();
		exit(1);
	}
}

void Audio::contextInfo(){
	int state = alGetInteger(AL_DISTANCE_MODEL);
	switch(state){
		case AL_NONE:
			std::cout << "AL_DISTANCE_MODEL: AL_NONE" << std::endl;
			break;
		case AL_INVERSE_DISTANCE:
				std::cout << "AL_DISTANCE_MODEL: AL_INVERSE_DISTANCE" << std::endl;
				break;
		case AL_INVERSE_DISTANCE_CLAMPED:
			std::cout << "AL_DISTANCE_MODEL: AL_INVERSE_DISTANCE_CLAMPED" << std::endl;
			break;
		case AL_LINEAR_DISTANCE:
			std::cout << "AL_DISTANCE_MODEL: AL_LINEAR_DISTANCE" << std::endl;
			break;
		case AL_LINEAR_DISTANCE_CLAMPED:
			std::cout << "AL_DISTANCE_MODEL: AL_LINEAR_DISTANCE_CLAMPED" << std::endl;
			break;
		case AL_EXPONENT_DISTANCE:
			std::cout << "AL_DISTANCE_MODEL: AL_EXPOENT_DISTANCE" << std::endl;
			break;
		case AL_EXPONENT_DISTANCE_CLAMPED:
			std::cout << "AL_DISTANCE_MODEL: AL_EXPOENT_DISTANCE_CLAMPED" << std::endl;
			break;
	}
}

void Audio::listenerGain(float gain){
	alListenerf(AL_GAIN, gain);
	displayError(alGetError(), "alListenerf<ALenum AL_GAIN>");
}

Audio::Audio(Audio_Container_Type flag, std::vector<Emitter>& emitters){
	type = flag;
	if(type == BGM)
		Audio::bgmStack.push_back(this);
	else if(type == SFX)
		Audio::sfxStack.push_back(this);
	setEmitters(emitters);
}

Audio::~Audio(){
	alDeleteSources(sources.size(), sources.data());
	displayError(alGetError(), "alDeleteSources");
	alDeleteBuffers(buffers.size(), buffers.data());
	displayError(alGetError(), "alDeleteBuffers");
}

void Audio::play(){
	if(!alcMakeContextCurrent(Audio::context)){
		std::cout << "Unable to create context!" << std::endl;
		exit(1);
	}
	for(uint i = 0; i < sources.size(); ++i){
		int state = -1;
		alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
		if(state == AL_INITIAL){
			alSourcePlay(sources[i]);
			displayError(alGetError());
		}
	}
}

void Audio::stop(){
	if(!alcMakeContextCurrent(Audio::context)){
		std::cout << "Unable to create context!" << std::endl;
		exit(1);
	}
	for(uint i = 0; i < sources.size(); ++i){
		int state = -1;
		alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
		if(state == AL_PLAYING){
			alSourceRewind(sources[i]);
			displayError(alGetError());
		}
	}
}


void Audio::selectSources(vec3<float>& position){
	if(type == SFX){
		for(uint i = 0; i < stackEmitters.size(); ++i){
			ALint state    = 0;
			float distance = length(position, stackEmitters[i].position);
			float range    = stackEmitters[i].range;
			alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
			if(state == AL_PAUSED)
				alSourcePlay(sources[i]);
			if(distance > range){
				alSourcePause(sources[i]);
			}
		}
	}
	else if(type == BGM){
		for(uint i = 0; i < stackEmitters.size(); ++i){
			ALint state     = 0;
			float distance  = length(position, stackEmitters[i].position);
			float range     = stackEmitters[i].range;
			float reference = stackEmitters[i].reference; 
			float lerp = distance > reference? distance: reference;
			lerp = distance < range? distance: range;
			stackEmitters[i].relgain = stackEmitters[i].gain - stackEmitters[i].rolloff * (lerp - reference) / (range - reference);
			stackEmitters[i].relgain = stackEmitters[i].relgain < 0.0f? 0.0f: stackEmitters[i].relgain;
			alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
			if(state == AL_STOPPED)
				alSourcePlay(sources[i]);
			if(distance < range & distance > reference){
				if(state == AL_INITIAL)
					alSourcePlay(sources[i]);
				alSourcef(sources[i], AL_GAIN, stackEmitters[i].relgain);
			}
			if(distance > range){
				alSourceStop(sources[i]);
			}
		}
	}
}

void Audio::updateSource(uint index, vec3<float> vector){
	stackEmitters[index].position = vector;
	alSourcefv(sources[index], AL_POSITION, stackEmitters[index].position.address());
	displayError(alGetError());
}


void Audio::renderSources(){
	for(uint i = 0; i < stackEmitters.size(); ++i)
		stackEmitters[i].render();
}

void Audio::setEmitters(std::vector<Emitter>& emitters){
	stackEmitters = emitters;
	buffers.resize(stackEmitters.size());
	sources.resize(stackEmitters.size());
	SDL_AudioSpec spec;
	Uint32 length = 0;
	Uint8 *buff   = NULL;
				
	for(uint i = 0; i < stackEmitters.size(); ++i){
		// - Load sample wav
		if(SDL_LoadWAV(stackEmitters[i].path.c_str(), &spec, &buff, &length) == NULL){
			std::cout << "Audio::Error: " << SDL_GetError() << std::endl;
			exit(1);
		}

		// - Generate and allocate
		alGenBuffers(1, &buffers[i]);
		displayError(alGetError(), "alGenBuffers");								
		alBufferData(buffers[i], stackEmitters[i].format, buff, length, spec.freq);
		displayError(alGetError(), "alBufferData");
		SDL_FreeWAV(buff);
		alGenSources(1, &sources[i]);
		displayError(alGetError(), "alGenSources");
		alSourcei(sources[i], AL_BUFFER, buffers[i]);
		displayError(alGetError(), "alSourcei");				
						
		// - Attenuation controls
		alSourcefv(sources[i], AL_POSITION, stackEmitters[i].position.address());
		displayError(alGetError(), "alSourcefv");	
		alSourcef(sources[i], AL_REFERENCE_DISTANCE, stackEmitters[i].reference);
		displayError(alGetError());					
		alSourcef(sources[i], AL_ROLLOFF_FACTOR, stackEmitters[i].rolloff);
		displayError(alGetError());					
		alSourcef(sources[i], AL_MAX_DISTANCE, stackEmitters[i].range);
		displayError(alGetError());					
		alSourcef(sources[i], AL_GAIN, stackEmitters[i].gain);
		displayError(alGetError(), "alSourcef:<ALenum AL_GAIN>");
		alSourcei(sources[i], AL_LOOPING, AL_TRUE);
		displayError(alGetError(), "alSourcei");					
		alSourcef(sources[i], AL_PITCH, 1.0f);
		displayError(alGetError(), "alSourcef:<ALenum AL_PITCH>");

		// - Direction sources
		if(stackEmitters[i].direction.x != 0.0f && stackEmitters[i].direction.y != 0.0f && stackEmitters[i].direction.z != 0.0f){
			alSourcefv(sources[i], AL_DIRECTION, stackEmitters[i].direction.address());
			alSourcef(sources[i], AL_CONE_INNER_ANGLE, stackEmitters[i].coneInnerAngle);
			alSourcef(sources[i], AL_CONE_OUTER_ANGLE, stackEmitters[i].coneOuterAngle);
			alSourcef(sources[i], AL_CONE_OUTER_GAIN, stackEmitters[i].coneOuterGain);
		}
	}
	buffersInfo();
}

void Audio::buffersInfo(){
	for(uint i = 0; i < buffers.size(); ++i){
		if(alIsBuffer(buffers[i])){
			int freq = -1;
			int size = -1;
			int bits = -1;
			int chnl = -1;
			alGetBufferi(buffers[i], AL_FREQUENCY, &freq);
			alGetBufferi(buffers[i], AL_SIZE, &size);
			alGetBufferi(buffers[i], AL_BITS, &bits);
			alGetBufferi(buffers[i], AL_CHANNELS, &chnl);
			std::cout << "ID: " << buffers[i] << " | " << freq << "hz | " << size / 1024 << "Kb | " << chnl << " channels" << std::endl;
		}
	}
}

std::vector<Emitter>& Audio::getEmitters(){
	return stackEmitters;
}

std::vector<uint>& Audio::getBuffers(){
	return buffers;
}

std::vector<uint>& Audio::getSources(){
	return sources;
}