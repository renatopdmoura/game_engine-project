#include "../headers/Core.hpp"
#include "../headers/math/Math.hpp"
#include "../headers/SRW.hpp"

using namespace srw;

void onStates(){
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** args){
	if(initialize(1200, 700)){
		writeOutputInfo.close();
		onStates();
		bool quit = false;
		uint programPanel   = SRW::genProgramID("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_ui_panel.glsl");
		uint programButton  = SRW::genProgramID("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_ui.glsl");
		uint programMinimal = SRW::genProgramID("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_ui_button.glsl");

		Panel lifeBar(0, 100, vec2f(20.0f, 3.0f), programPanel);
		lifeBar.setName(std::to_string((uint)std::round(lifeBar.getResolution().w / 50)), 98, 50, "MAIAN.ttf", 16.0f);
		lifeBar.bordered(vec3f(0.0f), 0.0f);
		lifeBar.fill(vec3f(1.0f, 0.0f, 0.0f));

		Panel end(50, 50, vec2f(100.0f, 30.0f), programPanel);
		end.setName("Game Over", 50, 90, "MAIAN.ttf", 24.0f);
		end.bordered(vec3f(0.0f), 0.0f);
		end.fill(vec3f(1.0f, 1.0f, 1.0f));
		end.opacity(0.2f);
		end.hident(true);

		//existe um falha estrutura que não renderiza texto em caso que não há nós descendentes
		Button gameover(50, 50, vec2f(12.0f, 20.0f), programMinimal, &end);
		gameover.setName("Ok", 50.0f, 50.0f, "MAIAN.ttf", 20.0f);
		gameover.addTexture("../assets/ui/ramp.jpg", "rampnull", 0, false, false);
		gameover.setUniform3f("fill", vec3f(0.2f));
		gameover.setUniform3f("border", vec3f(0.8f, 0.6f, 0.2f));
		gameover.setUniform1f("thickness", 2.0f);
		gameover.setUniform3f("blend", vec3f(1.0f));
		gameover.setUniform1b("clipping", false);
		gameover.pressed([&](void){
			gameover.setUniform3f(gameover.searchUniform3f("blend"), gameover.on);
			if(gameover.isApplied())
				quit = true;

		});
		gameover.hident(true);

		SpriteSheet title("../assets/ui/maintitle.png", 50, 50, 42, 26, 6);
		title.frameTime(32);
		title.frameTime(1, 1024);
		title.frameTime(5, 2048);
		title.frameTime(6, 2048);
		SpriteSheet::instances.pop_back();

		SpriteSheet tile1("../assets/ui/tiletex.jpg", 20.0f, 0.0f, 5, 0, 1, true);
		SpriteSheet tile2("../assets/ui/tiletex.jpg", 40.0f, 0.0f, 5, 0, 1, true);
		SpriteSheet tile3("../assets/ui/tiletex.jpg", 60.0f, 0.0f, 5, 0, 1, true);
		SpriteSheet tile4("../assets/ui/tiletex.jpg", 80.0f, 0.0f, 5, 0, 1, true);
		
		SpriteSheet cubox("../assets/ui/slime.png", 0, 50, 5, 0, 4, true);
		cubox.addSpriteSheet("../assets/ui/slime_w.png", SDL_SCANCODE_UP);
		cubox.addSpriteSheet("../assets/ui/slime_s.png", SDL_SCANCODE_DOWN);
		cubox.addSpriteSheet("../assets/ui/slime_d.png", SDL_SCANCODE_RIGHT);
		cubox.addSpriteSheet("../assets/ui/slime_a.png", SDL_SCANCODE_LEFT);
		cubox.addSpriteSheet("../assets/ui/slime_w.png", SDL_SCANCODE_SPACE);
		cubox.frameTime(64);
		SpriteSheet::instances.pop_back();

		SDL_Event event;
		while(!quit){
			while(SDL_PollEvent(&event) != 0){
				if(event.type == SDL_KEYDOWN){
					if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
						quit = true;
					}
				}
				if(event.type == SDL_WINDOWEVENT){
					if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
						UI::uiMatrix = ortho(screenWidth, 0.0f, screenHeight, 0.0f);
				}
				cubox.keyboardEvent(&event);
				gameover.windowEvent(&event);
				gameover.mouseEvent(&event);
			}	
			glViewport(0, 0, screenWidth, screenHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);		
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cubox.collision();
			if(cubox.edgeRight || cubox.edgeLeft || cubox.edgeTop || cubox.edgeBottom){
				//just detect only element at vector
				lifeBar.setScale(lifeBar.getScale().w - 4.0f, lifeBar.getScale().y);
				lifeBar.setName(std::to_string((uint)lifeBar.getResolution().w / 50));
				lifeBar.updatePosition();
				lifeBar.updateBuffer();
			}
			if(lifeBar.getScale().w <= 0.0f){
				end.hident(false);
				gameover.hident(false);
			}
			tile1.render();
			tile2.render();
			tile3.render();
			tile4.render();
			cubox.render();
			title.render();
			lifeBar.render();
			end.render();
			gameover.render();
			SDL_GL_SwapWindow(window);		
		}
	}
}