#include "../libraries/Common/Core.hpp"
#include "../libraries/Camera/Camera.hpp"
#include "../libraries/SRW/SRW.hpp"
#include "../libraries/Object/Object.hpp"
#include "../libraries/Terrain/Terrain.hpp"
#include "../libraries/Quad/Quad.hpp"
#include "../libraries/Text/Text.hpp"

#include "../libraries/Datatype/Datatype.hpp"
#include "../libraries/CommandLine/CommandLine.hpp"


#include <sstream>

bool QUIT             = false;
bool LOCKEDMOTION     = false;
bool COMMANDLINE      = false;
bool SENDCOMMANDLINE  = false;
int FRAME_PER_SECOND  = 60.0f;
int FRAME_TIME        = 1000.0f / FRAME_PER_SECOND;

int main(int argv, char** args){
	if(initialize("Pura Engine", 1280, 720, true)){
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glClearDepth(1.0f);
		glClearColor(0.2f, 0.2f, 0.2, 1.0f);

		mat4<float> uiMatrix = orthographic((float)ext_screen_width, 0.0f, (float)ext_screen_height, 0.0f);
		Text::canvasToClip = uiMatrix;
		Text readme("../assets/fonts/Humnst777.ttf", 18, 0, 50, vec3<float>(1.0f), "readme.txt");
		Text fpsMeter("../assets/fonts/Humnst777.ttf", 24, 0, 700, vec3<float>(0.0f, 1.0f, 0.0f), COUNTER);

		Camera fpsCam(vec3<float>(0.0f, 4.0f, 0.0f), vec3<float>(0.0f, -1.0f, -1.0f), 45.0f, 0.1f, 100.0f);
		uint gBuffer;
		uint gDepth;
		uint gPosition;
		uint gNormal;
		uint gAlbedo;
		SRW::genGeometryBuffer(gBuffer, gDepth, gPosition, gNormal, gAlbedo, ext_screen_width, ext_screen_height);
		
		Sun sun = {vec4<float>(0.0f, 1.0f, 0.0f), vec4<float>(0.2f, 0.2f, 0.2f)};
		std::vector<PointLight> pointLightArray = {{vec4<float>(0.0f, 2.0f, 8.0f),  vec4<float>(20.0f, 0.0f, 0.0f)},
										 		   {vec4<float>(8.0f, 2.0f, 0.0f),  vec4<float>(0.0f, 20.0f, 0.0f)},
										 		   {vec4<float>(0.0f, 2.0f, -8.0f), vec4<float>(0.0f, 0.0f, 20.0f)},
										 	  	   {vec4<float>(-8.0f, 2.0f, 0.0f), vec4<float>(0.0f, 20.0f, 20.0f)}};

		std::vector<SpotLight> spotLightArray   = {{vec4<float>(0.0f, 1.0f, 0.0f, 0.65f), vec4<float>(0.0f, 4.0f, 0.0f, 0.35f), vec4<float>(20.0f, 20.0f, 20.0f)}};
		SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), &sun, &pointLightArray, &spotLightArray);

		Material mtl000 = {vec3<float>(1.0f), 1.0f, 0.0f, 1.0f};
		Text cmdTxtIn("../assets/fonts/Humnst777.ttf", 24, 2, 20, vec3<float>(1.0f), DYNAMIC);
		CommandLine cmd(&mtl000, cmdTxtIn);

		// Terrain terrain("../assets/textures/terrain/maps/heightmap000.r8", 8, 256, 256, 20.0f, 1.0f);
		// terrain.addTexture("../assets/textures/grass/aerial_grass_rock_diff_2k.jpg", "material.albedo", 0, true, false);
		// terrain.addTexture("../assets/textures/grass/aerial_grass_rock_nor_gl_2k.jpg", "material.normal", 1, true, false);
		// terrain.addTexture("../assets/textures/grass/aerial_grass_rock_rough_2k.jpg", "material.roughness", 2, true, false);
		// terrain.addTexture("../assets/textures/grass/aerial_grass_rock_ao_2k.jpg", "material.ao", 3, true, false);
		// terrain.setUniform1f("material.metallic", 0.0f);
		// terrain.setUniform3f("cameraPos", &fpsCam.getPosition());
		// terrain.setUniformMat4f("viewProj", &fpsCam.getViewAndProjectionMatrix());
	
		std::vector<mat4<float>> arrayModels(100);
		Object obj000("../models/basic/icosphere.obj", UNIFORM_COLOR, &arrayModels, UNIFORM_BLOCK);
		obj000.getModel() = translate(vec3<float>(0.0f, 0.0f, 0.0f));
		obj000.setUniform3f("material.albedo", &mtl000.albedo);
		obj000.setUniform1f("material.roughness", &mtl000.roughness);
		obj000.setUniform1f("material.metallic", &mtl000.metallic);
		obj000.setUniform1f("material.ao", &mtl000.ao);
		
		Object obj001("../models/tree/birch_tree.obj", UNIFORM_COLOR);
		obj001.getModel() = scale(vec3<float>(2.0f));
		obj001.setUniform3f("material.albedo", &mtl000.albedo);
		obj001.setUniform1f("material.roughness", &mtl000.roughness);
		obj001.setUniform1f("material.metallic", &mtl000.metallic);
		obj001.setUniform1f("material.ao", &mtl000.ao);

		Object obj002("../models/ruins/knee-cap.obj", UNIFORM_COLOR);
		obj002.getModel() = scale(vec3<float>(1.0f, 1.0f, 1.0f));
		obj002.setUniform3f("material.albedo", &mtl000.albedo);
		obj002.setUniform1f("material.roughness", &mtl000.roughness);
		obj002.setUniform1f("material.metallic", &mtl000.metallic);
		obj002.setUniform1f("material.ao", &mtl000.ao);

		std::vector<mat4<float>> arrayColumns;
		for(uint j = 0; j < 2; ++j){
			for(uint i = 0; i < 4; ++i){
				float x = (float)(j * 32) + (-16);
				float z = (float)(i * 8) + (-16);
				arrayColumns.push_back(scale(vec3<float>(2.0f)) * translate(vec3<float>(x, 0, z)));
			}
		}
		Object obj003("../models/ruins/columns.obj", UNIFORM_COLOR, &arrayColumns, UNIFORM_BLOCK);
		obj003.getModel() = scale(vec3<float>(2.0f));
		obj003.setUniform3f("material.albedo", &mtl000.albedo);
		obj003.setUniform1f("material.roughness", &mtl000.roughness);
		obj003.setUniform1f("material.metallic", &mtl000.metallic);
		obj003.setUniform1f("material.ao", &mtl000.ao);

		Object obj004("../models/basic/plane.obj", UNIFORM_COLOR);
		obj004.getModel() = scale(vec3<float>(50.0f, 1.0f, 50.0f));
		obj004.setUniform3f("material.albedo", &mtl000.albedo);
		obj004.setUniform1f("material.roughness", &mtl000.roughness);
		obj004.setUniform1f("material.metallic", &mtl000.metallic);
		obj004.setUniform1f("material.ao", &mtl000.ao);
		
		Object::completeness();

		uint counted_frame = 0;
		uint elapsed_time  = SDL_GetTicks();
		uint faded_time    = 0;
		uint avgFps 	   = 0;
		bool applied       = 0;
		float theta        = 0.0f;
		
		SDL_Event event;
		while(!QUIT){
			while(SDL_PollEvent(&event) != 0){
				switch(event.type){
					case SDL_QUIT:
						QUIT = true;
						break;
					case SDL_KEYDOWN:
							if(event.key.keysym.scancode == SDL_SCANCODE_RETURN){
								COMMANDLINE  = !COMMANDLINE;
								LOCKEDMOTION = !LOCKEDMOTION;
							}
							if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
								LOCKEDMOTION = !LOCKEDMOTION;
							}
						break;
					case SDL_WINDOWEVENT:
						if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
							// uiMatrix = orthographic((float)event.window.data1, 0.0f, (float)event.window.data2, 0.0f);
							// Text::canvasToClip = uiMatrix;
							glDeleteTextures(1, &gPosition);
							glDeleteTextures(1, &gNormal);
							glDeleteTextures(1, &gAlbedo);
							glDeleteRenderbuffers(1, &gDepth);
							glDeleteFramebuffers(1, & gBuffer);
							SRW::genGeometryBuffer(gBuffer, gDepth, gPosition, gNormal, gAlbedo, (uint)event.window.data1, (uint)event.window.data2);
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
			// terrain.render();	
			theta += 0.75f;
			for(uint i = 0; i < 10; ++i){
				for(uint j = 0; j < 10; ++j){
					arrayModels[i * 10 + j] = scale(vec3<float>(0.5f)) * rotateX(theta) * rotateY(theta) * rotateZ(theta) * translate(vec3<float>((j * 10.0f) + (-50.0f), (i * 10.0f) + (-50.0f), i * 5.0f));
				}
			}
			obj000.updateInstances();
			obj000.render(arrayModels.size());
			obj003.render(arrayColumns.size());
			glUseProgram(SRW::programs[UNIFORM_COLOR]);
			obj001.render();
			obj002.render();
			obj004.render();
			glUseProgram(0);

			// - Text render
			cmd.render(COMMANDLINE? "[console]:": "");
			readme.render();
			avgFps = counted_frame / (SDL_GetTicks() / 1000.0f);
			fpsMeter.render(std::to_string(avgFps));
			// Text::renders();
			counted_frame++;
			SDL_GL_SwapWindow(ext_window);
		}
		obj000.free();
		obj001.free();
		obj002.free();
		obj003.free();
		obj004.free();
		// terrain.free();
		Text::clean();
		glDeleteTextures(1, &gPosition);
		glDeleteTextures(1, &gNormal);
		glDeleteTextures(1, &gAlbedo);
		glDeleteRenderbuffers(1, &gDepth);
		glDeleteFramebuffers(1, & gBuffer);
	}
	return 0;
}