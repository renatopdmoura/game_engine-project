// - Default pipeline headers
#include "../../Common/Core.hpp"
#include "../../SRW/SRW.hpp"
#include "../../Camera/Camera.hpp"
#include "../Object.hpp"

// - Independent headers
#include "../../Datatype/Datatype.hpp"

int main(int argv, char** args){
	if(Core::initialize("OpenGL", 800, 800)){
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glClearDepth(1.0f);

		Camera fpsCam(vec3<float>(0.0f, 0.0f, 0.0f), vec3<float>(0.0f, 0.0f, -1.0f), 45.0f, 0.1f, 100.0f);

		SRW::genProgramShaders();

		Sun sun = {vec4<float>(0.0f, 1.0f, 0.0f), vec4<float>(0.2f, 0.2f, 0.2f)};
		std::vector<PointLight> pointLightArray = {
			{vec4<float>(0.0f, 5.0f, 0.0f),  vec4<float>(80.0f, 80.0f, 80.0f)}
		};
		SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), &sun, &pointLightArray, NULL);

		Material mtl000 = {vec3<float>(1.0f), 1.0f, 0.0f, 1.0f};
					
		Object obj000("../../../models/basic/cube_offset", TEXTURIZED);
		obj000.setTextures("../../../assets/textures/rock/rock_pitted_mossy_diff_2k.jpg", "../../../assets/textures/rock/rock_pitted_mossy_nor_gl_2k.jpg", "../../../assets/textures/rock/rock_pitted_mossy_rough_2k.jpg", "../../../assets/textures/rock/rock_pitted_mossy_ao_2k.jpg", mtl000.metallic);		
			
		float angle = 0.0f;
		bool flag_smooth = false;
		uint frame = 0;
		uint loop_at = 16;
		
		SDL_Event event;
		
		uint frames_per_second = 10;
		uint skip_ticks        = 1000 / frames_per_second;
		uint next_game_ticks   = SDL_GetTicks();
		int sleep_time         = 0;

		uint prev_frame_tick = 0;
		uint curr_frame_tick = SDL_GetTicks();

		uint stabilize_fps = 0;
		
		while(!Core::Quit){
			// - Take 2 seconds to stabilize FPS
			if(stabilize_fps < 120){
				stabilize_fps++;
				continue;
			}
			while(SDL_PollEvent(&event) != 0){
				if(event.type == SDL_KEYDOWN){
					if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
						Core::Quit = true;
					if(event.key.keysym.scancode == SDL_SCANCODE_SPACE)
						flag_smooth = !flag_smooth;
				}
				fpsCam.keyboardEvent(&event);
				fpsCam.mouseEvent(&event);
				fpsCam.windowEvent(&event);
			}
			fpsCam.updateCoordinates();
			obj000.SpherexSphere(fpsCam);
			SRW::sharedUniforms(fpsCam.getViewAndProjectionMatrix(), fpsCam.getPosition(), 0);
			
			glViewport(0, 0, Core::ScreenWidth, Core::ScreenHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			angle++;
			glUseProgram(SRW::programs[TEXTURIZED]);			
			obj000.getModel() = rotateY(angle) * translate(vec3<float>(0.0f, 0.0f, -5.0f));
			for(uint i = 0; i < 1; ++i)
				obj000.render(frame, true, 5000, 2000);
			if(obj000.getNextTick() >= 5000 && obj000.getNextTick() <= 6000)
				frame = frame < 60? frame + 1: 59;			
			glUseProgram(0);

			// - Determina que cada quadro avance em skip_ticks
			// - Se a diferença entre o tempo de transição esperado pelo tempo registrado pelo real decorrido for menor que zero
			// - então estamos levando mais tempo para renderizar o que gostariamos.
			// next_game_ticks += skip_ticks;
			// sleep_time       = next_game_ticks - SDL_GetTicks();
			// if(sleep_time >= 0)
			// 	std::cout << "Sleeping...\n";
			// else
			// 	std::cout << "Delaying...\n";

			// - Calcula o tempo que leva para realizar a troca de frames
			prev_frame_tick = curr_frame_tick;
			curr_frame_tick = SDL_GetTicks();
			// std::cout << 1000 / (curr_frame_tick - prev_frame_tick) << std::endl;
			
			SDL_GL_SwapWindow(Core::Window);
		}
		obj000.free();	
	}
	return 0;
}