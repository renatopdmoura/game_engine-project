#include "../libraries/Common/Core.hpp"
#include "../libraries/Camera/Camera.hpp"
#include "../libraries/SRW/SRW.hpp"
#include "../libraries/Object/Object.hpp"
#include "../libraries/Terrain/Terrain.hpp"
#include "../libraries/Quad/Quad.hpp"
#include "../libraries/GUI/GeneralGUI.hpp"
#include "../libraries/Datatype/Datatype.hpp"
#include "../libraries/CommandLine/CommandLine.hpp"
#include "../libraries/Audio/Audio.hpp"

#include <sstream>

bool QUIT             	= false;
bool LOCKEDMOTION     	= false;
bool COMMANDLINE      	= false;
bool SENDCOMMANDLINE  	= false;
bool DEBUG_DEPTH      	= false;
bool DEBUG_NORMAL       = false;
bool DEBUG_GUI          = false;
bool SHOW_SOUND_EMITTER = false;
bool SHOWREADME         = false;
int FRAME_PER_SECOND    = 60.0f;
int FRAME_TIME          = 1000.0f / FRAME_PER_SECOND;

int main(int argv, char** args){
	if(initialize("Labs", 1366, 768, false, false)){
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glClearDepth(1.0f);
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

		uint benchmark = SDL_GetTicks();
	
		float zNear = 0.1f;
		float zFar  = 1200.0f;
		vec3<float> from(0.0f, 0.0f, 0.0f);
		vec3<float> to(0.0f, 0.0f, -1.0f);
		
		Audio::initialize(from, to, AL_INVERSE_DISTANCE_CLAMPED);
		
		Camera fpsCam(from, to, 45.0f, zNear, zFar);
		fpsCam.setSpeed(0.01f);
		std::vector<Emitter> charEmitter = {
			{Emitter("../assets/sfx/walking.wav", fpsCam.getPosition(), 1.0f, 1.0f, 1.0f, 0.0f, AL_FORMAT_STEREO16)}
		};
		Audio charSFX(SFX, charEmitter);
		fpsCam.sfx = &charSFX;
		
		std::vector<Emitter> tracks = {
			{Emitter("../assets/bgm/IntertwinedWorlds.wav", vec3<float>(0.0f, 0.0f, 0.0f), 1.0f, 96.0f, 1.0f, 0.1f, AL_FORMAT_STEREO16)}
		};
		Audio bgmList000(BGM, tracks);
		
		std::vector<Emitter> ambience = {
			// path, position, reference, range, rolloff, gain, format
			{Emitter("../assets/sfx/wind.wav", vec3<float>(0.0f, 1.0f, 16.0f), 1.0f, 24.0f, 0.15f, 1.0f, AL_FORMAT_MONO16)},
			{Emitter("../assets/sfx/cat.wav", vec3<float>(0.0f, 1.0f, -16.0f), 1.0f, 24.0f, 0.15f, 1.0f, AL_FORMAT_MONO16)},
			{Emitter("../assets/sfx/water.wav", vec3<float>(16.0f, 1.0f, 0.0f), 1.0f, 24.0f, 0.75f, 1.0f, AL_FORMAT_MONO16)},
			{Emitter("../assets/sfx/lumberjack.wav", vec3<float>(-16.0f, 1.0f, 0.0f), 1.0f, 24.0f, 1.0f, 1.0f, AL_FORMAT_MONO16)}
		};
		Audio sfxList000(SFX, ambience);		

		Sun sun = {vec4<float>(0.0f, 1.0f, 0.0f), vec4<float>(0.2f, 0.2f, 0.2f)};
		std::vector<PointLight> pointLightArray = {{vec4<float>(0.0f, 2.0f, 8.0f),  vec4<float>(80.0f, 0.0f, 0.0f)},
										 		   {vec4<float>(8.0f, 2.0f, 0.0f),  vec4<float>(0.0f, 80.0f, 0.0f)},
										 		   {vec4<float>(0.0f, 2.0f, -8.0f), vec4<float>(0.0f, 0.0f, 80.0f)},
										 		   {vec4<float>(-8.0f, 2.0f, 0.0f), vec4<float>(80.0f, 80.0f, 80.0f)}};

		std::vector<SpotLight> spotLightArray   = {{vec4<float>(0.0f, 1.0f, 0.0f, 0.65f), vec4<float>(0.0f, 4.0f, 0.0f, 0.35f), vec4<float>(20.0f, 20.0f, 20.0f)}};
		Material mtl000 = {vec3<float>(1.0f), 1.0f, 0.0f, 1.0f};

		SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), &sun, &pointLightArray, &spotLightArray);
		
		// - Terrain load			
		Terrain terrain("../assets/textures/terrain/maps/heightmap000.r8", 8, 256, 256, 40.0f, 2.0f);
		terrain.setBaseTex("forrest_ground_01_diff_1k.jpg", "forrest_ground_01_nor_1k.jpg", "forrest_ground_01_rough_1k.jpg", "forrest_ground_01_ao_1k.jpg");
		terrain.setSlopeTex("rock_wall_02_diff_1k.jpg", "rock_wall_02_nor_gl_1k.jpg", "rock_wall_02_rough_1k.jpg", "rock_wall_02_ao_1k.jpg");
		terrain.setPathTex("pathMask001.bmp", "grass_path_2_diff_1k.jpg", "grass_path_2_nor_1k.jpg", "grass_path_2_rough_1k.jpg", "grass_path_2_AO_1k.jpg");
		terrain.setUniform1f("material.metallic", &mtl000.metallic);
		std::cout << "[Terrain loaded]\n" << std::endl;

		// - 3D models
		std::vector<mat4<float>> arrayColumns;
		arrayColumns.push_back(scale(vec3<float>(0.25f)) * translate(vec3<float>(0.0f, 1.0f, -12.0f)));
		arrayColumns.push_back(scale(vec3<float>(0.25f)) * translate(vec3<float>(0.0f, 1.0f, 12.0f)));
		arrayColumns.push_back(scale(vec3<float>(0.25f)) * translate(vec3<float>(12.0f, 1.0f, 0.0f)));
		arrayColumns.push_back(scale(vec3<float>(0.25f)) * translate(vec3<float>(-12.0f, 1.0f, 0.0f)));
		Object obj000("../models/basic/sphere.obj", TEXTURIZED);
		obj000.getModel() = scale(vec3<float>(50.0f)) * rotateY(45.0f) * translate(vec3<float>(0.0f, 100.0f, -1000.0f));
		obj000.addTexture("../assets/textures/terrain/rock_wall_02_diff_1k.jpg", "material.albedo", 0);
		obj000.addTexture("../assets/textures/terrain/rock_wall_02_nor_gl_1k.jpg", "material.normal", 1);
		obj000.addTexture("../assets/textures/terrain/rock_wall_02_rough_1k.jpg", "material.roughness", 2);
		obj000.addTexture("../assets/textures/terrain/rock_wall_02_ao_1k.jpg", "material.ao", 3);
		obj000.setUniform1f("material.metallic", &mtl000.metallic);
		Object::completeness();
		std::cout << "[Models loaded]\n" << std::endl;
		
		// - GUI Object creation
		GUI::guiUniformBuffer();
		Text fpsMeter("../assets/fonts/Humnst777.ttf", 24.0f, 0.0f, 700.0f, vec3<float>(0.0f, 1.0f, 0.0f), COUNTER);
		Text cmdTxtIn("../assets/fonts/Humnst777.ttf", 24.0f, 2.0f, 20.0f, vec3<float>(1.0f), DYNAMIC);
		CommandLine cmd(&mtl000, cmdTxtIn);

		// - Panel Tools
		Panel panelTools(98.0f, 98.0f, 20.0f, 90.0f, 2.0f, vec4<float>(0.22f, 0.19f, 0.2f), vec4<float>(0.45f, 0.36f, 0.2f), vec4<float>(0.2f, 0.2f, 0.2f));
		Text titleTools("../assets/fonts/Humnst777.ttf", 18.0f, 2.0f, 50.0f, vec3<float>(0.9f), "TOOLS", ON_HEADER);
		Text txtHidden("../assets/fonts/Humnst777.ttf", 18.0f, 96.0f, 50.0f, vec3<float>(0.9f), "HIDEN", ON_HEADER);
		txtHidden.setUserEvent([&](){
			panelTools.setHidden(true);
		});
				
		// - Sub-Panel Debug
		Group cbxGroup;
		Panel panelDebug(50.0f, 100.0f, 100.0f, 33.0f, 6.0f, vec4<float>(vec4<float>(0.22f, 0.19f, 0.2f)), vec4<float>(0.25f, 0.2f, 0.34f), vec4<float>(0.0f), ON_BODY);
		Text titleDebug("../assets/fonts/Humnst777.ttf", 18.0f, 50.0f, 50.0f, vec3<float>(0.9f), "DEBUG", ON_HEADER);
		
		Text txtDebugDepth("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 90.0f, vec3<float>(0.9f), "DEBUG DEPTH", ON_BODY);
		Checkbox cbxDebugDepth(100.0f, 90.0f);
		cbxGroup.checkbox.push_back(&cbxDebugDepth);
		cbxDebugDepth.setUserEvent([&](){
			cbxGroup.doUnique(cbxDebugDepth.getID());
			if(!cbxDebugDepth.getOn()){
				cbxDebugDepth.setOn(true);
				cbxDebugDepth.setBackgroundColor(vec4<float>(1.0f, 1.0f, 1.0f));
			}
			else if(cbxDebugDepth.getOn()){
				cbxDebugDepth.setOn(false);
				cbxDebugDepth.setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			}
			#if RENDER_DEBUG_MODE
				DEBUG_NORMAL = false;
				if(!DEBUG_DEPTH){
					Object::renderDebugDepth(fpsCam.getViewAndProjectionMatrix(), fpsCam.getNear(), fpsCam.getFar());
					terrain.renderDebugDepth(fpsCam.getViewAndProjectionMatrix(), fpsCam.getNear(), fpsCam.getFar());
					DEBUG_DEPTH = true;
				}
				else
					DEBUG_DEPTH = false;
			#endif
		});
		
		Text txtDebugNormals("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 70.0f, vec3<float>(0.9f), "DEBUG NORMAL", ON_BODY);
		Checkbox cbxDebugNormals(100.0f, 70.0f);
		cbxGroup.checkbox.push_back(&cbxDebugNormals);
		cbxDebugNormals.setUserEvent([&](){
			cbxGroup.doUnique(cbxDebugNormals.getID());
			if(!cbxDebugNormals.getOn()){
				cbxDebugNormals.setOn(true);
				cbxDebugNormals.setBackgroundColor(vec4<float>(1.0f, 1.0f, 1.0f));
			}
			else if(cbxDebugNormals.getOn()){
				cbxDebugNormals.setOn(false);
				cbxDebugNormals.setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			}
			#if RENDER_DEBUG_MODE
				DEBUG_DEPTH = false;
				if(!DEBUG_NORMAL){
					Object::renderDebugNormal(fpsCam.getViewMatrix(), fpsCam.getProjectionMatrix());
					terrain.renderDebugNormal(fpsCam.getViewMatrix(), fpsCam.getProjectionMatrix());
					DEBUG_NORMAL = true;
				}
				else
					DEBUG_NORMAL = false;
			#endif
		});
		
		Text txtDebugGUI("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 50.0f, vec3<float>(0.9f), "DEBUG GUI", ON_BODY);
		Checkbox cbxDebugGUI(100.0f, 50.0f);
		cbxGroup.checkbox.push_back(&cbxDebugGUI);
		cbxDebugGUI.setUserEvent([&](){
			cbxGroup.doUnique(cbxDebugGUI.getID());
			if(!cbxDebugGUI.getOn()){
				cbxDebugGUI.setOn(true);
				cbxDebugGUI.setBackgroundColor(vec4<float>(1.0f, 1.0f, 1.0f));
			}
			else if(cbxDebugGUI.getOn()){
				cbxDebugGUI.setOn(false);
				cbxDebugGUI.setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			}
			DEBUG_GUI = !DEBUG_GUI;
		});

		Text txtDebugShading("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 30.0f, vec3<float>(0.9f), "SHADING GGX", ON_BODY);
		Checkbox cbxDebugShading(100.0f, 30.0f);
		
		Text txtDebugEmitters("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 10.0f, vec3<float>(0.9f), "DEBUG EMITTERS", ON_BODY);
		Checkbox cbxDebugEmitters(100.0f, 10.0f);
		cbxDebugEmitters.setUserEvent([&](){
			if(!cbxDebugEmitters.getOn()){
				cbxDebugEmitters.setOn(true);
				cbxDebugEmitters.setBackgroundColor(vec4<float>(1.0f, 1.0f, 1.0f));
			}
			else if(cbxDebugEmitters.getOn()){
				cbxDebugEmitters.setOn(false);
				cbxDebugEmitters.setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			}
			SHOW_SOUND_EMITTER = !SHOW_SOUND_EMITTER;
		});
	
		// - Panel Window 
		Panel panelWindow(50.0f, 50.0f, 100.0f, 33.0f, 6.0f, vec4<float>(0.22f, 0.19f, 0.2f), vec4<float>(0.25f, 0.2f, 0.34f), vec4<float>(0.0f), ON_BODY);
		Text titleWindow("../assets/fonts/Humnst777.ttf", 18.0f, 50.0f, 50.0f, vec3<float>(0.9f), "WINDOW", ON_HEADER);		
		Text wh1280x720("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 90.0f, vec3<float>(0.9f), "1280X720", ON_BODY);
		wh1280x720.setUserEvent([&](){
			SDL_SetWindowSize(ext_window, 1280, 720);
		});		
		Text wh1366x768("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 70.0f, vec3<float>(0.9f), "1366X768", ON_BODY);		
		wh1366x768.setUserEvent([&](){
			SDL_SetWindowSize(ext_window, 1366, 768);
		});		
		Text wh1400x1050("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 50.0f, vec3<float>(0.9f), "1400X1050", ON_BODY);
		wh1400x1050.setUserEvent([&](){
			SDL_SetWindowSize(ext_window, 1400, 1050);
		});		
		Text wh1600x900("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 30.0f, vec3<float>(0.9f), "1600X900", ON_BODY);
		wh1600x900.setUserEvent([&](){
			SDL_SetWindowSize(ext_window, 1600, 900);
		});		
		Text wh1920x1080("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 10.0f, vec3<float>(0.9f), "1920X1080", ON_BODY);		
		wh1920x1080.setUserEvent([&](){
			SDL_SetWindowSize(ext_window, 1920, 1080);
		});
		
		// - Panel Audio
		Panel panelAudio(50.0f, 0.0f, 100.0f, 33.0f, 6.0f, vec4<float>(0.22f, 0.19f, 0.2f), vec4<float>(0.25f, 0.2f, 0.34f), vec4<float>(0.0f), ON_BODY);
		Text titleAudio("../assets/fonts/Humnst777.ttf", 18.0f, 50.0f, 50.0f, vec3<float>(0.9f), "AUDIO", ON_HEADER);
		Text txtSwitchSFX("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 90.0f, vec3<float>(0.9f), "SFX", ON_BODY);
		Checkbox cbxSwitchSFX(100.0f, 90.0f);
		cbxSwitchSFX.setUserEvent([&](){
			if(!cbxSwitchSFX.getOn()){
				Audio::stopSFX();
				cbxSwitchSFX.setOn(true);
				cbxSwitchSFX.setBackgroundColor(vec4<float>(1.0f, 1.0f, 1.0f));
			}
			else if(cbxSwitchSFX.getOn()){
				Audio::playSFX();
				cbxSwitchSFX.setOn(false);
				cbxSwitchSFX.setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			}
		});
		Text txtSwitchBGM("../assets/fonts/Humnst777.ttf", 18.0f, 0.0f, 70.0f, vec3<float>(0.9f), "BGM", ON_BODY);
		Checkbox cbxSwitchBGM(100.0f, 70.0f);
		cbxSwitchBGM.setUserEvent([&](){
			if(!cbxSwitchBGM.getOn()){
				Audio::stopBGM();
				cbxSwitchBGM.setOn(true);
				cbxSwitchBGM.setBackgroundColor(vec4<float>(1.0f, 1.0f, 1.0f));
			}
			else if(cbxSwitchBGM.getOn()){
				Audio::playBGM();
				cbxSwitchBGM.setOn(false);
				cbxSwitchBGM.setBackgroundColor(vec4<float>(0.6f, 0.6f, 0.6f));
			}
		});
		Text txtTerminate("../assets/fonts/Humnst777.ttf", 18.0f, 50.0f, 5.0f, vec3<float>(0.9f), "TERMINATE", ON_BODY);	
		txtTerminate.setUserEvent([&](){
			QUIT = true;
		});	

		// - Connect in render order
		panelTools.push(&titleTools);
		panelTools.push(&txtHidden);
		panelTools.push(&panelDebug);
		panelTools.push(&panelWindow);
		panelTools.push(&panelAudio);

		panelDebug.push(&titleDebug);
		panelDebug.push(&txtDebugDepth);
		panelDebug.push(&cbxDebugDepth);
		panelDebug.push(&txtDebugNormals);
		panelDebug.push(&cbxDebugNormals);
		panelDebug.push(&txtDebugShading);
		panelDebug.push(&cbxDebugShading);
		panelDebug.push(&txtDebugGUI);
		panelDebug.push(&cbxDebugGUI);
		panelDebug.push(&txtDebugEmitters);
		panelDebug.push(&cbxDebugEmitters);

		panelWindow.push(&titleWindow);
		panelWindow.push(&wh1280x720);
		panelWindow.push(&wh1366x768);		
		panelWindow.push(&wh1400x1050);		
		panelWindow.push(&wh1600x900);
		panelWindow.push(&wh1920x1080);				

		panelAudio.push(&titleAudio);
		panelAudio.push(&txtSwitchSFX);
		panelAudio.push(&cbxSwitchSFX);
		panelAudio.push(&txtSwitchBGM);
		panelAudio.push(&cbxSwitchBGM);
		panelAudio.push(&txtTerminate);
		
		std::cout << "[Master UI Objects: " << GUI::stack.size() << "]\n" << std::endl;
		
		benchmark = SDL_GetTicks();
		std::cout << "Time to load assets: " << benchmark << std::endl;	

		uint counted_frame = 0;
		uint elapsed_time  = SDL_GetTicks();
		uint faded_time    = 0;
		uint avgFps 	   = 0;
		bool applied       = 0;
		float theta        = 0.0f;
		float offset       = 0;
		
		SDL_Event event;
		Audio::playAll();
		while(!QUIT){
			GUI::stackPicking();
			while(SDL_PollEvent(&event) != 0){
				switch(event.type){
					case SDL_QUIT:
						QUIT = true;
						break;
					case SDL_KEYDOWN:
							switch(event.key.keysym.scancode){
								case SDL_SCANCODE_RETURN:
									COMMANDLINE  = !COMMANDLINE;
									LOCKEDMOTION = !LOCKEDMOTION;
									break;
								case SDL_SCANCODE_ESCAPE:
									LOCKEDMOTION = !LOCKEDMOTION;
									break;
								case SDL_SCANCODE_F2:
									panelTools.setHidden(false);
									break;
							}
						break;
					case SDL_WINDOWEVENT:
						if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
							SDL_GetWindowSize(ext_window, &ext_screen_width, &ext_screen_height);
							SDL_SetWindowPosition(ext_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
						}
						break;
				}
				if(!LOCKEDMOTION){
					fpsCam.keyboardEvent(&event);
					fpsCam.mouseEvent(&event);
					SDL_SetRelativeMouseMode(SDL_TRUE);
					cmd.setMaterial(cmd.strInput);
					cmd.strInput = "";
				}
				else{
					SDL_SetRelativeMouseMode(SDL_FALSE);
					if(COMMANDLINE){
						cmd.write(true);
						cmd.input->keyboardEvent(&event, cmd.strInput);
					}
					else{
						cmd.write(false);
					}
					GUI::stackPollEvent(&event);
				}
				fpsCam.windowEvent(&event);
				Text::events(&event);
			}
			fpsCam.updateCoordinates();
			SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), 0);
			
			// - Collision detection
			// terrain.getHeightAt(fpsCam.getPosition());
			
			// - Play ambient sounds
			Audio::updateListener(fpsCam.getPosition(), fpsCam.getTarget());
			Audio::selectByDistance(fpsCam.getPosition());			
					
			glViewport(0, 0, ext_screen_width, ext_screen_height);
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// - Object transform
			// sfxList000.updateSource(0, fpsCam.getPosition() + vec3<float>(cos(theta) * 6.0f, sin(theta), sin(theta) * 6.0f));
			
			// - Object render
			// obj000.render(arrayColumns.size());

			glEnable(GL_DEPTH_TEST);		
			#if RENDER_DEBUG_MODE
				if(SHOW_SOUND_EMITTER){
					bgmList000.renderSources();
					sfxList000.renderSources();
				}
				if(DEBUG_DEPTH){
					glUseProgram(SRW::debugPrograms[0]);
					terrain.render();	
					// obj000.render();
					glUseProgram(0);
				}
				else if(DEBUG_NORMAL){
					glUseProgram(SRW::debugPrograms[1]);
					terrain.render();	
					// obj000.render();
					glUseProgram(0);
				}
				if(!DEBUG_DEPTH){
					glUseProgram(SRW::programs[TERRAIN_PROGRAM]);
					terrain.render();	
					glUseProgram(SRW::programs[TEXTURIZED]);
					obj000.render();
					glUseProgram(0);					
				}
			#else
				glUseProgram(SRW::programs[TERRAIN_PROGRAM]);
				terrain.render();	
				glUseProgram(SRW::programs[TEXTURIZED]);
				// obj000.render();
				glUseProgram(0);
			#endif

			glDisable(GL_DEPTH_TEST);			
			// - Text render
			cmd.render(COMMANDLINE? "[console]:": "");
			avgFps = counted_frame / (SDL_GetTicks() / 1000.0f);
			fpsMeter.render(std::to_string(avgFps));
			if(!DEBUG_GUI)
				GUI::stackRender();
			else
				GUI::stackPicking();
			counted_frame++;
			SDL_GL_SwapWindow(ext_window);
		}
		obj000.free();
		terrain.free();
		Audio::free();
	}
	return 0;
}