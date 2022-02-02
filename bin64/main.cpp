
/*
-> Corrigir a classe Terrain
*/


#include "../libraries/Common/Core.hpp"
#include "../libraries/Camera/Camera.hpp"
#include "../libraries/SRW/SRW.hpp"
#include "../libraries/Object/Object.hpp"
#include "../libraries/Terrain/Terrain.hpp"
#include "../libraries/Quad/Quad.hpp"
#include "../libraries/Text/Text.hpp"
#include "../libraries/GUI/GUI.hpp"

#include "../libraries/Datatype/Datatype.hpp"
#include "../libraries/CommandLine/CommandLine.hpp"

#include <sstream>
bool QUIT             = false;
bool LOCKEDMOTION     = false;
bool COMMANDLINE      = false;
bool SENDCOMMANDLINE  = false;
bool DEBUG_DEPTH      = false;
bool DEBUG_NORMAL     = false;
bool SHOWREADME       = false;
int FRAME_PER_SECOND  = 60.0f;
int FRAME_TIME        = 1000.0f / FRAME_PER_SECOND;

int main(int argv, char** args){
	if(initialize("Labs", 1280, 720, true)){
		// glEnable(GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glClearDepth(1.0f);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		uint benchmark = SDL_GetTicks();
	
		float zNear = 0.1f;
		float zFar  = 200.0f;
		Camera fpsCam(vec3<float>(0.0f, 4.0f, 0.0f), vec3<float>(0.0f, -1.0f, -1.0f), 45.0f, zNear, zFar);

		Sun sun = {vec4<float>(0.0f, 1.0f, 0.0f), vec4<float>(0.2f, 0.2f, 0.2f)};
		std::vector<PointLight> pointLightArray = {{vec4<float>(0.0f, 2.0f, 8.0f),  vec4<float>(80.0f, 0.0f, 0.0f)},
										 		   {vec4<float>(8.0f, 2.0f, 0.0f),  vec4<float>(0.0f, 80.0f, 0.0f)},
										 		   {vec4<float>(0.0f, 2.0f, -8.0f), vec4<float>(0.0f, 0.0f, 80.0f)},
										 		   {vec4<float>(-8.0f, 2.0f, 0.0f), vec4<float>(80.0f, 80.0f, 80.0f)}};

		std::vector<SpotLight> spotLightArray   = {{vec4<float>(0.0f, 1.0f, 0.0f, 0.65f), vec4<float>(0.0f, 4.0f, 0.0f, 0.35f), vec4<float>(20.0f, 20.0f, 20.0f)}};
		
		SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), &sun, &pointLightArray, &spotLightArray);
		// SRW::genProgramShaders();
	
		mat4<float> uiMatrix = orthographic((float)ext_screen_width, 0.0f, (float)ext_screen_height, 0.0f);
		Text::canvasToClip = uiMatrix;
		Text readme("../assets/fonts/Humnst777.ttf", 18, 50, 50, vec3<float>(1.0f), "readme.txt");
		Text fpsMeter("../assets/fonts/Humnst777.ttf", 24, 0, 700, vec3<float>(0.0f, 1.0f, 0.0f), COUNTER);

		Material mtl000 = {vec3<float>(1.0f), 1.0f, 0.0f, 1.0f};
		Text cmdTxtIn("../assets/fonts/Humnst777.ttf", 24, 2, 20, vec3<float>(1.0f), DYNAMIC);
		CommandLine cmd(&mtl000, cmdTxtIn);

		Panel panel000(0.0f, 0.0f, 0.0f, 0.0f, vec2<float>(0.25f, 0.25f), vec3<float>(0.4f, 0.25f, 0.25f));
		Panel panel001(0.0f, 0.0f, 0.0f, 0.0f, vec2<float>(-0.25f, -0.25f), vec3<float>(0.5f, 0.75f, 0.45f));
		Panel panel002(0.0f, 0.0f, 0.0f, 0.0f, vec2<float>(-0.25f, 0.45f), vec3<float>(0.062f, 0.065f, 0.6f));
		Panel panel003(0.0f, 0.0f, 0.0f, 0.0f, vec2<float>(0.5f, 0.45f), vec3<float>(0.752f, 0.752f, 0.752f));
		Panel panel004(0.0f, 0.0f, 0.0f, 0.0f, vec2<float>(-0.5f, 0.45f), vec3<float>(0.12f, 0.4f, 0.6f));
		
		Terrain terrain("../assets/textures/terrain/maps/heightmap000.r8", 8, 256, 256, 40.0f, 2.0f);
		terrain.addTexture("../assets/textures/terrain/snow_02_diff_1k.jpg", "material.albedo[0]", 0);
		terrain.addTexture("../assets/textures/terrain/rock_wall_02_diff_1k.jpg", "material.albedo[1]", 1);

		terrain.addTexture("../assets/textures/terrain/snow_02_nor_1k.jpg", "material.normal[0]", 2);
		terrain.addTexture("../assets/textures/terrain/rock_wall_02_nor_gl_1k.jpg", "material.normal[1]", 3);
		
		terrain.addTexture("../assets/textures/terrain/snow_02_rough_1k.jpg", "material.roughness[0]", 4);
		terrain.addTexture("../assets/textures/terrain/rock_wall_02_rough_1k.jpg", "material.roughness[1]", 5);
		
		terrain.addTexture("../assets/textures/terrain/snow_02_ao_1k.jpg", "material.ao[0]", 6);
		terrain.addTexture("../assets/textures/terrain/rock_wall_02_ao_1k.jpg", "material.ao[1]", 7);
		
		terrain.addTexture("../assets/textures/terrain/paths/pathMask001.bmp", "material.pathMask", 8);
		terrain.addTexture("../assets/textures/terrain/grass_path_2_diff_1k.jpg", "material.albedo[2]", 9);
		terrain.addTexture("../assets/textures/terrain/grass_path_2_nor_1k.jpg", "material.normal[2]", 10);
		terrain.addTexture("../assets/textures/terrain/grass_path_2_rough_1k.jpg", "material.roughness[2]", 11);
		terrain.addTexture("../assets/textures/terrain/grass_path_2_AO_1k.jpg", "material.ao[2]", 12);

		terrain.setUniform1f("material.metallic", &mtl000.metallic);
	
		std::vector<mat4<float>> arrayModels(100);
		Object obj000("../models/basic/icosphere.obj", UNIFORM_COLOR, &arrayModels, UNIFORM_BLOCK);
		obj000.getModel() = translate(vec3<float>(0.0f, 0.0f, 0.0f));
		obj000.setUniform3f("material.albedo", &mtl000.albedo);
		obj000.setUniform1f("material.roughness", &mtl000.roughness);
		obj000.setUniform1f("material.metallic", &mtl000.metallic);
		obj000.setUniform1f("material.ao", &mtl000.ao);
		Object::stack.pop_back();
		
		Object obj001("../models/tree/birch_tree.obj", UNIFORM_COLOR);
		obj001.getModel() = scale(vec3<float>(4.0f));
		obj001.setUniform3f("material.albedo", &mtl000.albedo);
		obj001.setUniform1f("material.roughness", &mtl000.roughness);
		obj001.setUniform1f("material.metallic", &mtl000.metallic);
		obj001.setUniform1f("material.ao", &mtl000.ao);

		Object obj002("../models/basic/icosphere.obj", TEXTURIZED);
		obj002.getModel() = scale(vec3<float>(1.0f, 1.0f, 1.0f)) * translate(vec3<float>(0.0f, 2.0f, 0.0f));
		obj002.addTexture("../assets/textures/terrain/wood_table_001_diff_1k.jpg", "material.albedo", 0);
		obj002.addTexture("../assets/textures/terrain/wood_table_001_nor_gl_1k.jpg", "material.normal", 1);
		obj002.addTexture("../assets/textures/terrain/wood_table_001_rough_1k.jpg", "material.roughness", 2);
		obj002.addTexture("../assets/textures/terrain/wood_table_001_ao_1k.jpg", "material.ao", 3);
		obj002.setUniform1f("material.metallic", &mtl000.metallic);

		std::vector<mat4<float>> arrayColumns;
		arrayColumns.push_back(scale(vec3<float>(30.0f)) * rotateY(45.0f) * translate(vec3<float>(-170, -6.0f, -50)));
		

		Object obj003("../models/ruins/stone.obj", TEXTURIZED, &arrayColumns, UNIFORM_BLOCK);
		obj003.addTexture("../assets/textures/terrain/rock_wall_02_diff_1k.jpg", "material.albedo", 0);
		obj003.addTexture("../assets/textures/terrain/rock_wall_02_nor_gl_1k.jpg", "material.normal", 1);
		obj003.addTexture("../assets/textures/terrain/rock_wall_02_rough_1k.jpg", "material.roughness", 2);
		obj003.addTexture("../assets/textures/terrain/rock_wall_02_ao_1k.jpg", "material.ao", 3);
		obj003.setUniform1f("material.metallic", &mtl000.metallic);
		Object::stack.pop_back();

		Object::completeness();
		
		benchmark = SDL_GetTicks();
		std::cout << "Time to load assets: " << benchmark << std::endl;	

		uint counted_frame = 0;
		uint elapsed_time  = SDL_GetTicks();
		uint faded_time    = 0;
		uint avgFps 	   = 0;
		bool applied       = 0;
		float theta        = 0.0f;
		float offset = 0;
		
		SDL_Event event;
		while(!QUIT){
			
			glViewport(0, 0, ext_screen_width, ext_screen_height);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(SRW::programs[GUI_PROGRAM]);
			GUI::stackPicking();
			glUseProgram(0);

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
								case SDL_SCANCODE_F1:
									SHOWREADME = !SHOWREADME;
									break;
							#if RENDER_DEBUG_MODE
								case SDL_SCANCODE_F11:
									DEBUG_DEPTH = false;
									if(!DEBUG_NORMAL){
										Object::renderDebugNormal(fpsCam.getViewMatrix(), fpsCam.getProjectionMatrix());
										terrain.renderDebugNormal(fpsCam.getViewMatrix(), fpsCam.getProjectionMatrix());
										DEBUG_NORMAL = true;
									}
									else
										DEBUG_NORMAL = false;
									break;
								case SDL_SCANCODE_F12:
									DEBUG_NORMAL = false;
									if(!DEBUG_DEPTH){
										Object::renderDebugDepth(fpsCam.getViewAndProjectionMatrix(), zNear, zFar);
										terrain.renderDebugDepth(fpsCam.getViewAndProjectionMatrix(), zNear, zFar);
										DEBUG_DEPTH = true;
									}
									else
										DEBUG_DEPTH = false;
									break;
							#endif
							}
						break;
					case SDL_WINDOWEVENT:
						if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
							// uiMatrix = orthographic((float)event.window.data1, 0.0f, (float)event.window.data2, 0.0f);
							// Text::canvasToClip = uiMatrix;
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
					panel000.mouseEvent(&event);
					panel001.mouseEvent(&event);
					panel002.mouseEvent(&event);
					panel003.mouseEvent(&event);
					panel004.mouseEvent(&event);
				}
				fpsCam.windowEvent(&event);
				Text::events(&event);
			}
			faded_time += applied? 16: 0;
			fpsCam.updateCoordinates();
			SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), 0);
			// terrain.getHeightAt(fpsCam.getPosition());
					
			glViewport(0, 0, ext_screen_width, ext_screen_height);
	
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// - Object render
			theta += 0.75f;
			for(uint i = 0; i < 10; ++i){
				for(uint j = 0; j < 10; ++j){
					arrayModels[i * 10 + j] = scale(vec3<float>(0.5f)) * rotateX(theta) * rotateY(theta) * rotateZ(theta) * translate(vec3<float>((j * 10.0f) + (-50.0f), 20.0f, (i * 10.0f) + (-50.0f)));
				}
			}
			obj000.updateInstances();
			obj000.render(arrayModels.size());
			glEnable(GL_DEPTH_TEST);
			obj003.render(arrayColumns.size());
			#if RENDER_DEBUG_MODE
				if(DEBUG_DEPTH){
					glUseProgram(SRW::debugPrograms[0]);
					terrain.render();	
					obj001.render();
					obj002.render();
					glUseProgram(0);
				}
				else if(DEBUG_NORMAL){
					glUseProgram(SRW::debugPrograms[1]);
					terrain.render();	
					obj001.render();
					obj002.render();
					glUseProgram(0);
				}
				if(DEBUG_NORMAL || !DEBUG_DEPTH){
					glUseProgram(SRW::programs[TERRAIN_PROGRAM]);
					terrain.render();	
					glUseProgram(0);
					// glUseProgram(SRW::programs[UNIFORM_COLOR]);
					// obj001.render();
					// glUseProgram(0);
					glUseProgram(SRW::programs[TEXTURIZED]);
					obj002.render();
					glUseProgram(0);
				}
			#else
				glUseProgram(SRW::programs[TERRAIN_PROGRAM]);
				terrain.render();	
				glUseProgram(SRW::programs[UNIFORM_COLOR]);
				obj001.render();
				glUseProgram(0);
				// glUseProgram(SRW::programs[TEXTURIZED]);
				// obj002.render();
				// glUseProgram(0);
			#endif

			glDisable(GL_DEPTH_TEST);			
			// - Text render
			cmd.render(COMMANDLINE? "[console]:": "");
			avgFps = counted_frame / (SDL_GetTicks() / 1000.0f);
			fpsMeter.render(std::to_string(avgFps));
			if(SHOWREADME) 
				readme.render();
			// Text::renders();

			GUI::stackRender();

			counted_frame++;
			SDL_GL_SwapWindow(ext_window);
		}
		obj000.free();
		obj001.free();
		obj002.free();
		obj003.free();
		// terrain.free();
		Text::clean();
	}
	return 0;
}