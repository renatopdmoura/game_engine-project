#include "../headers/Core.hpp"
#include "../headers/math/Math.hpp"
#include "../headers/SRW.hpp"
#include "../headers/world/Terrain.hpp"
#include <cmath>

void keyboardEvent(SDL_Event* event);
void move();

bool quit              = false;
bool render_debug_quad = false;
bool LOCKEDMOTION      = false;
float angle            = 0.0f;
mat4<float> viewMatrixUI;
mat4<float> lightSpaceMatrix;

void onStates(){
	//Habilita a definição de tamanho da primitiva ponto
	glEnable(GL_PROGRAM_POINT_SIZE);
	//Habilita exclusão de faces traseiras
	glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW); 
	glCullFace(GL_BACK);
    //Habilita teste de profundidade
   
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
	glClearDepth(1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glEnable(GL_STENCIL_TEST);		
	//Gama correction method 1
	// glEnable(GL_FRAMEBUFFER_SRGB);
	//Ant-alising
	// glEnable(GL_MULTISAMPLE);
}

int main(int argc, char** args){
	//Inicializa o OpenGL
	if(initialize(1280, 720)){
		writeOutputInfo.close();
		onStates();		
		Camera camera(0.0f, 1.0f, 0.0f, 60.0f, 0.2f, 200.0f);

		uint programPanel   = SRW::genProgramID("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_ui_panel.glsl");
		uint programButton  = SRW::genProgramID("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_ui.glsl");
		uint programMinimal = SRW::genProgramID("../shaders/ui/vs_ui.glsl", "../shaders/ui/fs_ui_button.glsl");
	
		srw::Button cursor(50, 50, vec2f(1, 0), programButton);
		cursor.setUniform3f("blend", vec3f(0.8f));
		cursor.addTexture("../assets/ui/cursor.png", "icon", 0, false, false);

		srw::Panel menu(50.0f, 0.0f, vec2f(15.0f, 4.0f), programPanel);
		menu.bordered(vec3f(0.8f, 0.6f, 0.2f), 2.0f, GRADIENT, ALL_BORDER);
		menu.fill(vec3f(0.1f));
		
		//Begin Settings
		Panel tabSettings(0.0f, 50.0f, vec2f(30.0f, 100.0f), programPanel);
		tabSettings.bordered(vec3f(0.0f), 0.0f);
		tabSettings.fill(vec3f(0.0f));
		tabSettings.opacity(0.0f);
		tabSettings.hident(true);

		Panel toolbarSettings(0.0f, 100.0f, vec2f(100.0f, 6.0f), programPanel);
		toolbarSettings.setName("Settings", 5.0f, 50.0f, "MAIAN.ttf", 20.0f);
		toolbarSettings.setParent(&tabSettings);
		toolbarSettings.bordered(vec3f(0.8f, 0.6f, 0.2f), 2.0f, GRADIENT, BOTTOM_BORDER);
		toolbarSettings.fill(vec3f(0.1f, 0.1f, 0.1f));
		
		srw::Button closeSettings(95, 50, vec2f(2.5f, 0.0f), programButton);
		closeSettings.setParent(&toolbarSettings);
		closeSettings.addTexture("../assets/ui/close.png", "icon", 0, false, false);
		closeSettings.setUniform3f("blend", vec3f(0.0f));
		closeSettings.pressed([&](void){
				closeSettings.setUniform3f(closeSettings.searchUniform3f("blend"), closeSettings.on);
				if(closeSettings.isApplied()){
					closeSettings.terminate();
				}
			}
		);

		Panel sidebar(0.0f, 0.0f, vec2f(100.0f, 4.0f), programPanel, &tabSettings);
		sidebar.bordered(vec3f(0.8f, 0.6f, 0.2f), 2.0f, GRADIENT, ALL_BORDER);
		sidebar.fill(vec3f(0.1f, 0.1f, 0.1f));
			
		Panel tabNews(50.0f, 40.0f, vec2f(100.0f, 90.5f), programPanel, &tabSettings);
		tabNews.bordered(vec3f(0.0f), 0.0f, UNIFORM, BORDERLESS);
		tabNews.fill(vec3f(0.1f, 0.1f, 0.2f));
		tabNews.opacity(0.5f);
		
		Panel tabWindow(50.0f, 40.0f, vec2f(100.0f, 90.5f), programPanel, &tabSettings);
		tabWindow.bordered(vec3f(0.0f), 0.0f, UNIFORM, BORDERLESS);
		tabWindow.fill(vec3f(0.1f, 0.1f, 0.2f));
		tabWindow.opacity(0.5f);
		
		Panel tabMessage(50.0f, 40.0f, vec2f(100.0f, 90.5f), programPanel, &tabSettings);
		tabMessage.bordered(vec3f(0.8f, 0.8f, 0.8f), 2.0f, UNIFORM, BORDERLESS);
		tabMessage.fill(vec3f(0.8f, 0.8f, 0.8f));
		tabMessage.opacity(0.5f);

		Panel sectionWindowMode(50.0f, 100.0f, vec2f(90.0f, 33.0f), programPanel, &tabWindow);
		sectionWindowMode.setName("Window Mode", 2.0f, 95.0f, NULL, 24.0f);
		sectionWindowMode.bordered(vec3f(0.8f, 0.6f, 0.2f), 2.0f, GRADIENT, BOTTOM_BORDER);
		sectionWindowMode.fill(vec3f(0.1f, 0.1f, 0.1f));
		sectionWindowMode.opacity(0.0f);
		
		Panel sectionResolution(50.0f, 50.0f, vec2f(90.0f, 33.0f), programPanel, &tabWindow);
		sectionResolution.setName("Resolution", 2.0f, 95.0f, NULL, 24.0f);
		sectionResolution.bordered(vec3f(0.8f, 0.6f, 0.2f), 2.0f, GRADIENT, BOTTOM_BORDER);
		sectionResolution.fill(vec3f(0.1f, 0.1f, 0.1f));
		sectionResolution.opacity(0.0f);

		Panel sectionAdjustment(50.0f, 0.0f, vec2f(90.0f, 33.0f), programPanel, &tabWindow);
		sectionAdjustment.setName("Adjustment", 2.0f, 95.0f, NULL, 24.0f);
		sectionAdjustment.bordered(vec3f(0.8f, 0.6f, 0.2f), 2.0f, GRADIENT, ALL_BORDER);
		sectionAdjustment.fill(vec3f(0.1f, 0.1f, 0.1f));
		sectionAdjustment.opacity(0.0f);

		Button btnGBuffer(10.0f, 80.0f, vec2f(15.0f, 0), programButton, &sectionAdjustment);
		btnGBuffer.addTexture("../assets/ui/gBuffer.png", "icon", 0, false, false);
		btnGBuffer.setUniform3f("blend", vec3f(0.8f));
		btnGBuffer.pressed([&](void){
				btnGBuffer.setUniform3f(btnGBuffer.searchUniform3f("blend"), vec3f(0.6f));
				if(btnGBuffer.isApplied()){
					render_debug_quad = !render_debug_quad;
				}
			}
		);

		Button btnFovyPlus(30.0f, 80.0f, vec2f(15.0f, 0), programButton, &sectionAdjustment);
		btnFovyPlus.addTexture("../assets/ui/plus.png", "icon", 0, false, false);
		btnFovyPlus.setUniform3f("blend", vec3f(0.8f));
		btnFovyPlus.pressed([&](void){
				btnFovyPlus.setUniform3f(btnFovyPlus.searchUniform3f("blend"), vec3f(0.6f));
				if(btnFovyPlus.isApplied()){
					camera.setFovy(camera.getFovy() + 0.25f);
				}
			}
		);
		Button btnFovyMinus(50.0f, 80.0f, vec2f(15.0f, 0), programButton, &sectionAdjustment);
		btnFovyMinus.addTexture("../assets/ui/minus.png", "icon", 0, false, false);
		btnFovyMinus.setUniform3f("blend", vec3f(0.8f));
		btnFovyMinus.pressed([&](void){
				btnFovyMinus.setUniform3f(btnFovyMinus.searchUniform3f("blend"), vec3f(0.6f));
				if(btnFovyMinus.isApplied()){
					camera.setFovy(camera.getFovy() - 0.25f);
				}
			}
		);

		srw::Button btnFullscreen(35.0f, 50.0f, vec2f(15.0f, 0.0f), programButton);
		btnFullscreen.addTexture("../assets/ui/fullscreen.png", "icon", 0, false, false);
		btnFullscreen.setUniform3f("blend", vec3f(0.8f));
		btnFullscreen.setParent(&sectionWindowMode);
		btnFullscreen.pressed([&](void){
				btnFullscreen.setUniform3f(btnFullscreen.searchUniform3f("blend"), vec3f(0.6f));
				if(btnFullscreen.isApplied()){
					SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				}
			}
		);

		srw::Button btnWindowMode(65, 50, vec2f(15, 0), programButton);
		btnWindowMode.addTexture("../assets/ui/windowmode.png", "icon", 0, false, false);
		btnWindowMode.setUniform3f("blend", vec3f(0.8f));
		btnWindowMode.setParent(&sectionWindowMode);
		btnWindowMode.pressed([&](void){
				btnWindowMode.setUniform3f(btnWindowMode.searchUniform3f("blend"), vec3f(0.6f));
				if(btnWindowMode.isApplied()){
					SDL_SetWindowFullscreen(window, 0);
				}
			}
		);

		srw::Button w1200xh768(50, 75, vec2f(24.0f, 7.0f), programMinimal, &sectionResolution);
		w1200xh768.setName("1200x768", 50, 50, "MAIAN.ttf", 15.0f);
		w1200xh768.addTexture("../assets/ui/ramp.jpg", "rampTex", 0, false, false);
		w1200xh768.setUniform3f("fill", vec3f(0.2f));
		w1200xh768.setUniform3f("border", vec3f(0.8f, 0.6f, 0.2f));
		w1200xh768.setUniform1f("thickness", 2.0f);
		w1200xh768.setUniform3f("blend", vec3f(0.8f));
		w1200xh768.setUniform1b("clipping", true);
		w1200xh768.pressed([&](void){
			w1200xh768.setUniform3f(w1200xh768.searchUniform3f("blend"), w1200xh768.on);
			if(w1200xh768.isApplied())
				if(screenWidth != 1200 || screenHeight != 768){
					SDL_SetWindowSize(window, 1200, 768);
					SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
				}
		});

		srw::Button w1920xh1080(50, 60, vec2f(24.0f, 7.0f), programMinimal, &sectionResolution);
		w1920xh1080.setName("1920x1080", 50, 50, "MAIAN.ttf", 15.0f);
		w1920xh1080.addTexture("../assets/ui/ramp.jpg", "rampTex", 0, false, false);
		w1920xh1080.setUniform3f("fill", vec3f(0.2f));
		w1920xh1080.setUniform3f("border", vec3f(0.8f, 0.6f, 0.2f));
		w1920xh1080.setUniform1f("thickness", 2.0f);
		w1920xh1080.setUniform3f("blend", vec3f(0.8f));
		w1920xh1080.setUniform1b("clipping", true);
		w1920xh1080.pressed([&](void){
			w1920xh1080.setUniform3f(w1920xh1080.searchUniform3f("blend"), w1920xh1080.on);
			if(w1920xh1080.isApplied())
				if(screenWidth != 1920 || screenHeight != 1080){
					SDL_SetWindowSize(window, 1920, 1080);
					SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
				}
		});

		tabWindow.scrollbar(true, vec2f(100, 200));

		srw::Button btnNews(19, 50, vec2f(5.0f, 0), programButton);
		btnNews.addTexture("../assets/ui/news.png", "icon", 0, false, false);
		btnNews.setUniform3f("blend", vec3f(0.0f));
		btnNews.setParent(&sidebar);
		btnNews.summon(&tabWindow);
		btnNews.pressed([&](void){
				btnNews.setUniform3f(btnNews.searchUniform3f("blend"), vec3f(0.6f));
				if(btnNews.isApplied()){
					tabNews.hident(false);
					//Encerra as seguintes janelas se estiverem abertas
					if(!tabWindow.isHidden()) tabWindow.hident(true);
					if(!tabMessage.isHidden()) tabMessage.hident(true);
				}
			}
		);

		srw::Button btnWindow(1, 50, vec2f(5.0f, 0), programButton);
		btnWindow.addTexture("../assets/ui/window.png", "icon", 0, false, false);
		btnWindow.setUniform3f("blend", vec3f(0.0f));
		btnWindow.setParent(&sidebar);
		btnWindow.summon(&tabWindow);
		btnWindow.pressed([&](void){
				btnWindow.setUniform3f(btnWindow.searchUniform3f("blend"), vec3f(0.6f));
				if(btnWindow.isApplied()){
					tabWindow.hident(false);
					//Encerra as seguintes janelas se estiverem abertas
					if(!tabNews.isHidden()) tabNews.hident(true);
					if(!tabMessage.isHidden()) tabMessage.hident(true);
				}
			}
		);

		srw::Button btnMessage(10, 50, vec2f(5.0f, 0), programButton);
		btnMessage.addTexture("../assets/ui/message.png", "icon", 0, false, false);
		btnMessage.setUniform3f("blend", vec3f(0.0f));
		btnMessage.setParent(&sidebar);
		btnMessage.pressed([&](void){
				btnMessage.setUniform3f(btnMessage.searchUniform3f("blend"), vec3f(0.6f));
				if(btnMessage.isApplied()){
					tabMessage.hident(false);
					//Encerra as seguintes janelas se estiverem abertas
					if(!tabNews.isHidden()) tabNews.hident(true);
					if(!tabWindow.isHidden()) tabWindow.hident(true);
				}
			}
		);

		srw::Button settings(10.0f, 50.0f, vec2f(10.0f, 0.0f), programButton, &menu);
		settings.addTexture("../assets/ui/settings.png", "icon", 0, false, false);
		settings.setUniform3f("blend", vec3f(0.8f));
		settings.summon(&tabSettings);
		settings.pressed([&](void){
				settings.setUniform3f(settings.searchUniform3f("blend"), settings.on);
				if(settings.isApplied()){
					settings.enableUI = false;
					UI::lockedUI = !UI::lockedUI;
					settings.getSummon()->hident(!settings.getSummon()->isHidden());
					if(!tabSettings.isHidden()){
						tabWindow.hident(tabWindow.isHidden());
						tabNews.hident(!tabNews.isHidden());
						tabMessage.hident(!tabMessage.isHidden());
					}
				}
			}
		);
		//End Settings

		Panel tabHelp(2.0f, 50.0f, vec2f(100.0f, 100.0f), programPanel);
		tabHelp.bordered(vec3f(0.0f), 0.0f);
		tabHelp.fill(vec3f(0.1f));
		tabHelp.hident(true);

		Panel toolbarHelp(0.0f, 100.0f, vec2f(100.0f, 6.0f), programPanel);
		toolbarHelp.setName("Help", 1.0f, 50.0f, NULL, 20.0f);
		toolbarHelp.setParent(&tabHelp);
		toolbarHelp.bordered(vec3f(0.0f), 0.0f);
		toolbarHelp.fill(vec3f(0.1f));
				
		srw::Button closeHelp(99, 50, vec2f(1, 0), programButton);
		closeHelp.setParent(&toolbarHelp);
		closeHelp.addTexture("../assets/ui/close.png", "icon", 0, false, false);
		closeHelp.setUniform3f("blend", closeHelp.out);
		closeHelp.pressed([&](void){
				closeHelp.setUniform3f(closeSettings.searchUniform3f("blend"), closeHelp.on);
				if(closeHelp.isApplied()){
					closeHelp.terminate();
				}
			}
		);

		srw::Button help(50, 50.0f, vec2f(10.0f, 0.0f), programButton, &menu);
		help.addTexture("../assets/ui/help.png", "icon", 0, false, false);
		help.setUniform3f("blend", vec3f(0.8f));
		help.summon(&tabHelp);
		help.pressed([&](void){
				help.setUniform3f(help.searchUniform3f("blend"), help.on);
				if(help.isApplied()){
					help.enableUI = false;
					UI::lockedUI = !UI::lockedUI;
					help.getSummon()->hident(!help.getSummon()->isHidden());
				}
			}
		);

		srw::Button exit(90.0f, 50.0f, vec2f(10.0, 0), programButton, &menu);
		exit.addTexture("../assets/ui/exit.png", "icon", 0, false, false);
		exit.setUniform3f("blend", vec3f(0.8f));
		exit.pressed([&](void){
				exit.setUniform3f(exit.searchUniform3f("blend"), vec3f(0.6f));
				quit = exit.isApplied()? true: false;
			}
		);

		QueueUI queueUI;

		//Menu objects
		queueUI.put(menu);
		queueUI.put(exit);
		queueUI.put(settings);
		queueUI.put(help);
				
		//Tab settings objects
		queueUI.put(tabSettings);
		queueUI.put(tabNews);
		queueUI.put(tabWindow);
		queueUI.put(tabMessage);
		queueUI.put(sectionWindowMode);
		queueUI.put(sectionResolution);
		queueUI.put(sectionAdjustment);
		queueUI.put(btnFullscreen);
		queueUI.put(btnWindowMode);
		queueUI.put(btnFovyPlus);
		queueUI.put(btnFovyMinus);
		queueUI.put(btnGBuffer);
		queueUI.put(w1200xh768);
		queueUI.put(w1920xh1080);
		queueUI.put(toolbarSettings);
		queueUI.put(closeSettings);
		queueUI.put(sidebar);
		queueUI.put(btnNews);
		queueUI.put(btnMessage);
		queueUI.put(btnWindow);

		//Tab help objects
		queueUI.put(tabHelp);
		queueUI.put(toolbarHelp);
		queueUI.put(closeHelp);

		Skybox skybox("../assets/textures/skybox/montain", 0.3f);

		//Begin scene configuration
		Light sun("Sun", vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 5.0f, 0.0f), vec3f(0.15f, 0.15f, 0.15f), vec3f(0.25f, 0.25f, 0.25f), vec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
		Light redLight("Red", vec3f(0.0f, -1.0f, 0.0f), vec3f(-44.0f, 4.0f, 0.0f), vec3f(0.2f, 0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f);
		Light greenLight("Green", vec3f(0.0f, -1.0f, 0.0f), vec3f(44.0f, 4.0f, 0.0f), vec3f(0.0f, 0.2f, 0.0f), vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f), 1.0f, 0.09f, 0.032f);
		Light blueLight("Blue", vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 4.0f, -44.0f), vec3f(0.0f, 0.0f, 0.2f), vec3f(0.0f, 0.0f, 1.0f), vec3f(0.0f, 0.0f, 1.0f), 1.0f, 0.09f, 0.032f);
		Light whiteLight("White", vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 4.0f, 44.0f), vec3f(0.2f, 0.2f, 0.2f), vec3f(1.0f, 1.0f, 1.0f), vec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.22f, 0.2f);

		Scene scene000;
		scene000.setWorldLight(sun);		
		scene000.addLight(redLight);
		scene000.addLight(greenLight);
		scene000.addLight(blueLight);
		scene000.addLight(whiteLight);
		#if WRITE_IN_SHADER
		scene000.genLightInstances();
		#endif
		uint progDeferred            = SRW::genProgramID("../shaders/deferred/vs_deferred.glsl", "../shaders/deferred/fs_deferred.glsl");
		uint progSDL 		 		 = SRW::genProgramID("../shaders/shadow/vs_shadow_map.glsl", "../shaders/shadow/fs_shadow_map.glsl");
		uint progSPL  				 = SRW::genProgramID("../shaders/shadow/vs_pls.glsl", "../shaders/shadow/gs_pls.glsl", "../shaders/shadow/fs_pls.glsl");
		uint progMinimapID   	     = SRW::genProgramID("../shaders/minimap/vs_minimap_obj.glsl", "../shaders/minimap/fs_minimap_obj.glsl");
		uint progTerrainMinimapID  	 = SRW::genProgramID("../shaders/minimap/vs_minimap_obj.glsl", "../shaders/minimap/fs_minimap_terrain.glsl");
		uint programID 			   	 = SRW::genProgramID("../shaders/standart/vs_standart.glsl", "../shaders/standart/fs_standart.glsl", &scene000);
		
		//Gera os framebuffers para renderização dos mapas de sombras
		vec2uint res_shadow_map = vec2uint(1024, 1024);
		//Directional shadow map ids
		uint fboDirShadowMap = 0;
		uint texDirShadowMap = 0;
		SRW::genShadowFramebuffer(texDirShadowMap, fboDirShadowMap, res_shadow_map.w, res_shadow_map.h);

		//Omnidirectional shadow map ids		
		uint depthCubemapFBO = 0; 
		uint texDepthCubemap = 0; 
		float zNear = 0.01f, zFar = 100.0f;
		SRW::genShadowFramebufferFromCubemap(texDepthCubemap, depthCubemapFBO, res_shadow_map.h, res_shadow_map.h);
		
		mat4<float> shadowProjection = perspective(90.0f, (float)screenWidth / (float)screenHeight, zNear, zFar);
		std::vector<mat4<float>> shadowTransforms;
		shadowTransforms.push_back(lookAtRH(whiteLight.getPosition(), whiteLight.getPosition() + vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f)) * shadowProjection);
		shadowTransforms.push_back(lookAtRH(whiteLight.getPosition(), whiteLight.getPosition() + vec3f(-1.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f)) * shadowProjection);
		shadowTransforms.push_back(lookAtRH(whiteLight.getPosition(), whiteLight.getPosition() + vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f)) * shadowProjection);
		shadowTransforms.push_back(lookAtRH(whiteLight.getPosition(), whiteLight.getPosition() + vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 0.0f, -1.0f)) * shadowProjection);
		shadowTransforms.push_back(lookAtRH(whiteLight.getPosition(), whiteLight.getPosition() + vec3f(0.0f, 0.0f, 1.0f), vec3f(0.0f, -1.0f, 0.0f)) * shadowProjection);
		shadowTransforms.push_back(lookAtRH(whiteLight.getPosition(), whiteLight.getPosition() + vec3f(0.0f, 0.0f, -1.0f), vec3f(0.0f, -1.0f, 0.0f)) * shadowProjection);

		//Gera o framebuffer para seleção de objetos com base na cor do pixel
		uint texSelector, rboSelector, framebufferSelector;
		SRW::genFramebuffer(texSelector, rboSelector, framebufferSelector, screenWidth, screenHeight);

		//Gera o buffer para renderização adiada (G-Buffer)
		uint gBuffer = 0, gPosition = 0, gNormal = 0, gAlbedo = 0;
		SRW::genGeometryBuffer(gBuffer, gPosition, gNormal, gAlbedo, screenWidth, screenHeight);

		mat4<float> cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * translate(vec3f(20.0f, 0.45f, 2.0));
		Object cube("models/basic/cube.obj", progDeferred);
		cube.setDiffuseMap("box_diff.png");
		cube.setSpecularMap("box_spec.png");
		cube.setNormalMap("box_norm.jpg");
		cube.setUniform1f("material.shininess", 32.0f);
		cube.setUniformMat4fv("modelMatrix", &cube_transform);
		cube.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());

		mat4<float> gate_transform = scale(vec3f(6.0f, 6.0f, 6.0f)) * rotateY(90.0f) * translate(vec3f(-44.0f, 5.8f, 0.0));
		Object gate("models/gate.obj", progDeferred);
		gate.addTexture("../assets/textures/brick2.jpg", "material.diffuse", 0, true, false);
		gate.addTexture("../assets/textures/brick2_spec.jpg", "material.specular", 1, false, true);
		gate.addTexture("../assets/textures/brick2_normal_map.jpg", "material.normal", 3, false, true);
		gate.addTexture("../assets/textures/brick2_depth_map.bmp", "material.depth", 2, false, true);
		gate.setUniform1f("material.shininess", 32.0f);
		gate.setUniformMat4fv("modelMatrix", &gate_transform);
		gate.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());
		// gate.setUniform3fv("camerapos", &camera.getPosition());
		// gate.addTexture(texDepthMap, "depthMap", 4);
		// gate.addTexture(texDepthCubemap, "depthCubemap", 5, 1);
		// gate.setUniform1f("sHeight", &sHeightUnif);
		// gate.setUniform1f("zFar", zFar);
		// gate.setUniformMat4fv("lightSpaceMatrix", &lightSpaceMatrix);

		mat4<float> post_transform = scale(vec3f(2.0f, 2.0f, 2.0f)) * translate(vec3f(0.0f, 0.0f, 0.0f));
		Object post("models/lightpost.obj", progDeferred);
		post.addTexture("../assets/textures/stone_wall/diff/tex_diff_02.jpg", "material.diffuse", 0, true, false);
		post.addTexture("../assets/textures/stone_wall/spec/tex_spec_02.jpg", "material.specular", 1, true, false);
		post.addTexture("../assets/textures/stone_wall/disp/tex_disp_02.jpg", "material.depth", 2, true, false);
		post.addTexture("../assets/textures/stone_wall/norm/tex_norm_02.jpg", "material.normal", 3, true, false);
		post.setUniform1f("material.shininess", 32.0f);
		post.setUniformMat4fv("modelMatrix", &post_transform);
		post.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());
		// post.setUniform3fv("camerapos", &camera.getPosition());
		// post.addTexture(texDepthMap, "depthMap", 4);
		// post.addTexture(texDepthCubemap, "depthCubemap", 5, 1);
		// post.setUniform1f("sHeight", &sHeightUnif);
		// post.setUniform1f("zFar", zFar);
		// post.setUniformMat4fv("lightSpaceMatrix", &lightSpaceMatrix);

		mat4<float> pave_transform = scale(vec3f(4.0f, 4.0f, 4.0f));
		Object pave("models/square.obj", progDeferred);
		pave.setDiffuseMap("stone_wall/diff/tex_diff_01.jpg");
		pave.setSpecularMap("stone_wall/spec/tex_spec_01.jpg");
		pave.setNormalMap("stone_wall/norm/tex_norm_01.jpg");
		pave.setDisplacementMap("stone_wall/disp/tex_disp_02.jpg");
		pave.setUniform1f("material.shininess", 32.0f);
		pave.setUniformMat4fv("modelMatrix", &pave_transform);
		pave.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());
		// wall.addTexture(texDepthMap, "depthMap", 4);
		// pave.setUniform3fv("camerapos", &camera.getPosition());
		// wall.addTexture(texDepthCubemap, "depthCubemap", 5, 1);
		// wall.setUniform1f("sHeight", &sHeightUnif);
		// wall.setUniform1f("zFar", zFar);
		// wall.setUniformMat4fv("lightSpaceMatrix", &lightSpaceMatrix);

		mat4<float> sun_transform = scale(vec3f(0.25f, 0.25f, 0.25f)) * translate(sun.getPosition());
		Object sunCaster("models/basic/sphere.obj", "../shaders/standart/vs_just_color.glsl", "../shaders/standart/fs_just_color.glsl");
		sunCaster.setUniform3fv("color", &sun.getDiffuse());
		sunCaster.setUniformMat4fv("modelMatrix", &sun_transform);
		sunCaster.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());

		mat4<float> red_transform = scale(vec3f(0.25f, 0.25f, 0.25f)) * translate(redLight.getPosition());
		Object redCaster("models/basic/sphere.obj", "../shaders/standart/vs_just_color.glsl", "../shaders/standart/fs_just_color.glsl");
		redCaster.setUniform3fv("color", &redLight.getDiffuse());
		redCaster.setUniformMat4fv("modelMatrix", &red_transform);
		redCaster.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());

		mat4<float> green_transform = scale(vec3f(0.25f, 0.25f, 0.25f)) * translate(greenLight.getPosition());
		Object greenCaster("models/basic/sphere.obj", "../shaders/standart/vs_just_color.glsl", "../shaders/standart/fs_just_color.glsl");
		greenCaster.setUniform3fv("color", &greenLight.getDiffuse());
		greenCaster.setUniformMat4fv("modelMatrix", &green_transform);
		greenCaster.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());

		mat4<float> blue_transform = scale(vec3f(0.25f, 0.25f, 0.25f)) * translate(blueLight.getPosition());
		Object blueCaster("models/basic/sphere.obj", "../shaders/standart/vs_just_color.glsl", "../shaders/standart/fs_just_color.glsl");
		blueCaster.setUniform3fv("color", &blueLight.getDiffuse());
		blueCaster.setUniformMat4fv("modelMatrix", &blue_transform);
		blueCaster.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());

		mat4<float> white_transform = scale(vec3f(0.25f)) * translate(whiteLight.getPosition());
		Object whiteCaster("models/basic/sphere.obj", "../shaders/standart/vs_just_color.glsl", "../shaders/standart/fs_just_color.glsl");
		whiteCaster.setUniform3fv("color", &whiteLight.getDiffuse());
		whiteCaster.setUniformMat4fv("modelMatrix", &white_transform);
		whiteCaster.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());

		Quad renderQuad(0);
		renderQuad.setCurrentScene(scene000);
		renderQuad.addTexture(gPosition, "gBuffer.position", 0);
		renderQuad.addTexture(gNormal, "gBuffer.normal", 1);
		renderQuad.addTexture(gAlbedo, "gBuffer.albedo", 2);
		renderQuad.addTexture(gAlbedo, "gBuffer.specular", 3);
		renderQuad.addTexture(texDirShadowMap, "texDirShadowMap", 4);
		renderQuad.setUniform3fv("camerapos", &camera.getPosition());
		renderQuad.setUniformMat4fv("lightSpaceMatrix", &lightSpaceMatrix);
		Quad quad_position_debug(1);
		quad_position_debug.addTexture(gPosition, "tex", 0);
		Quad quad_normal_debug(1);
		quad_normal_debug.addTexture(gNormal, "tex", 0);
		Quad quad_albedo_debug(1);
		quad_albedo_debug.addTexture(gAlbedo, "tex", 0);
		Quad quad_shadow_debug(2);
		quad_shadow_debug.addTexture(texDirShadowMap, "tex", 0);

	#if DEBUG
		Object boundingBox;
		boundingBox.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());
	#endif

		mat4<float> terrainMatrix = translate(vec3f(0.0f, -10.0f, 0.0f));
		Terrain terrain(20.0f, 2.0f, scene000);
		terrain.loadHeightMap("../assets/textures/terrain/paths/silhouette_256x256_8bpp.r8", 8, 256, 256);
		terrain.fx();
		terrain.addTexture("../assets/textures/terrain/forrest_ground_01_diff_1k.jpg", "material.texLayer0Diff", 0, true, false);
		terrain.addTexture("../assets/textures/terrain/forrest_ground_01_ao_1k.jpg", "material.texLayer0Spec", 1, true, false);
		terrain.addTexture("../assets/textures/terrain/forrest_ground_01_nor_1k.jpg", "material.texLayer0Norm", 2, true, false);
		terrain.addTexture("../assets/textures/terrain/aerial_rocks_02_diff_1k.jpg", "material.texLayer1Diff", 3, true, false);
		terrain.addTexture("../assets/textures/terrain/rock_3_AO_1k.jpg", "material.texLayer1Spec", 4, true, false);
		terrain.addTexture("../assets/textures/terrain/aerial_rocks_02_nor_1k.jpg", "material.texLayer1Norm", 5, true, false);
		terrain.addTexture("../assets/textures/terrain/paths/pathMapGround.jpg", "material.texLayerPath0Map",  6, true, false);
		terrain.addTexture("../assets/textures/terrain/grass_path_2_diff_1k.jpg", "material.texLayerPath0Diff", 7, true, false);
		terrain.addTexture("../assets/textures/terrain/grass_path_2_AO_1k.jpg", "material.texLayerPath0Spec", 8, true, false);
		terrain.addTexture("../assets/textures/terrain/paths/pathMapSnow.jpg", "material.texLayerPath1Map",  9, true, false);
		terrain.addTexture("../assets/textures/terrain/snow_02_diff_1k.jpg", "material.texLayerPath1Diff", 10, true, false);
		terrain.addTexture("../assets/textures/terrain/snow_02_ao_1k.jpg", "material.texLayerPath1Spec", 11, true, false);	
		terrain.addTexture(texDirShadowMap, "depthMap", 12);
		terrain.addTexture(texDepthCubemap, "depthCubemap", 13);
		terrain.setUniform3fv("camerapos", &camera.getPosition());
		terrain.setUniformMat4fv("modelMatrix", &terrainMatrix);
		terrain.setUniformMat4fv("viewMatrix", &camera.getViewAndProjectionMatrix());
		terrain.setUniformMat4fv("lightSpaceMatrix", &lightSpaceMatrix);
		uint progTerrainID = terrain.getProgram();
	#if DEBUG_VARIABLES_HEIGHTMAP
		// terrain.memory_usage();
		// terrain.info();
	#endif
		//End srw
		CSM csm;
	
		//"Game Loop"
		SDL_Event event;
		while(!quit){
			UI::uiMatrix = ortho(screenWidth, 0.0f, screenHeight, 0.0f);
			camera.updateCoordinates();	
			Skybox::viewAndProjection = orientation(camera.getViewMatrix()) * camera.getProjectionMatrix();
			while(SDL_PollEvent(&event) != 0){
				if(event.type == SDL_QUIT){
					quit = true;
				}
				if(event.type == SDL_WINDOWEVENT){
					if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
						UI::uiMatrix = ortho(screenWidth, 0.0f, screenHeight, 0.0f);
					}
				}
				//UI events
				cursor.windowEvent(&event);
				if(!LOCKEDMOTION){
				#if DEBUG
					Object::picking(&event);
				#endif
					queueUI.poll(&event);
				#if DEBUG_VARIABLES_HEIGHTMAP
					terrain.keyboardEvent(&event);
				#endif
				}
				else{
					camera.keyboardEvent(&event);
					camera.mouseEvent(&event);
				}
				keyboardEvent(&event);
				camera.windowEvent(&event);
			}
			// csm.calculateLayerBoundaries(camera.getViewAndProjectionMatrix());
			// lightSpaceMatrix = csm.light;
			lightSpaceMatrix = lookAtRH(vec3f(camera.getPosition().x, 15.0f, 0.0f), vec3f(camera.getPosition().x, 15.0f, 0.0f) + vec3f(-1.0f, -1.0f, 0.0f)) * orthographic(20.0f, -20.0f, 20.0f, -20.0f, 1.0f, 16.0f);
			sun_transform = scale(vec3f(0.25f)) * translate(vec3f(camera.getPosition().x, 15.0f, 0.0f));


			glViewport(0, 0, res_shadow_map.w, res_shadow_map.h);
			glBindFramebuffer(GL_FRAMEBUFFER, fboDirShadowMap);
			glClear(GL_DEPTH_BUFFER_BIT);
			pave.renderFromDirectionalLightView(progSDL, lightSpaceMatrix, pave_transform);
			post.renderFromDirectionalLightView(progSDL, lightSpaceMatrix, post_transform);
			gate.renderFromDirectionalLightView(progSDL, lightSpaceMatrix, gate_transform);
			
			cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * translate(vec3f(20.0f, 0.45f, 2.0));
			cube.renderFromDirectionalLightView(progSDL, lightSpaceMatrix, cube_transform);
			cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * translate(vec3f(10.0f, 0.45f, 4.0));
			cube.renderFromDirectionalLightView(progSDL, lightSpaceMatrix, cube_transform);
			cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * rotateY(angle++) * translate(vec3f(3.0f, 5.0f, 12.0));
			cube.renderFromDirectionalLightView(progSDL, lightSpaceMatrix, cube_transform);


			glViewport(0, 0, screenWidth, screenHeight);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		#if DEBUG
			// SECTION: Color Picking
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			boundingBox.renderColorPicking();
		#endif
			//SECTION: G-Buffer
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			
			pave.render();
			gate.render();
			post.render();	
			cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * translate(vec3f(20.0f, 0.45f, 2.0));
			cube.render();
			cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * translate(vec3f(10.0f, 0.45f, 4.0));
			cube.render();
			cube_transform = scale(vec3f(0.45f, 0.45f, 0.45f)) * rotateY(angle++) * translate(vec3f(3.0f, 5.0f, 12.0));
			cube.render();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 		
    		float x = (screenWidth  - screenWidth / 2)  / 2.0f;
    		float y = (screenHeight - screenHeight / 2) / 2.0f;
    		renderQuad.render(0, 0, screenWidth, screenHeight);

    		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    		glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
    		sunCaster.render();
    		whiteCaster.render();
    		redCaster.render();
    		greenCaster.render();
    		blueCaster.render();
    		skybox.render();

			if(render_debug_quad){
				glDisable(GL_DEPTH_TEST);
	    		float w = screenWidth  / 8.0f;
	    		float h = w;
	    		y = (screenHeight - h) - menu.getResolution().h;
		 		quad_position_debug.render(screenWidth - w - 12, (((screenHeight - h) / 2) + 2 * h - h / 2) + 24, w, h);
		 		quad_normal_debug.render(screenWidth - w - 12, (((screenHeight - h) / 2) + h - h / 2) + 8, w, h);
		 		quad_albedo_debug.render(screenWidth - w - 12, (((screenHeight - h) / 2) - h + h / 2) - 8, w, h);
		 		quad_shadow_debug.render(screenWidth - w - 12, (((screenHeight - h) / 2) - 2 * h + h / 2) - 24, w, h);
	 		}

			//Render UI
			glEnable(GL_BLEND);
			if(LOCKEDMOTION){
				cursor.render();
			}
			queueUI.render();
			glDisable(GL_BLEND);
			SDL_GL_SwapWindow(window);
		}
		queueUI.free();
		terrain.free();
	}
	shutdown();
	return 0;
}

void keyboardEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN){
		switch(event->key.keysym.scancode){
 			case SDL_SCANCODE_ESCAPE:
 				LOCKEDMOTION = !LOCKEDMOTION;
 				if(LOCKEDMOTION){
 					SDL_SetRelativeMouseMode(SDL_TRUE);
					SDL_SetWindowGrab(window, SDL_TRUE);
					//Retomar a ultima posição do mouse pela diferencça entre "latestMousePos"
					//e currentMousePos 	
				}
				else{
					SDL_SetRelativeMouseMode(SDL_FALSE);
					SDL_SetWindowGrab(window, SDL_FALSE);
					SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
 					//Obter a posição atual do mouse "latestMousePos"
				}
 			break;
 			default:
 				break;
 		}
	}
}


/*
Ao renderizar um ponto 'p' transformado por uma matriz 't', que transforma o ponto do ponto de vista da luz, determinamos
que os pontos cuja profundidade igualam-se a 1* estão vísiveis.
Ao renderizar o mesmo ponto 'p', agora transformado por uma matriz 't', que transforma um ponto do ponto de vista do observador 
e comparar o valor da coordenada z desse ponto nessa passagem de renderização com o framebuffer gerado na primeira passagem de renderização ocorre que:
	- O ponto 'p' que estiver ocluido do ponto de vista da luz, por efeito de oclusão, não terá sua coordenada z gravada no framebuffer;
	- Tratando a questão do ponto de vista do observador, pode-se dizer que o ponto 'p' ocluido, salvo no buffer de profundidade pela passagem de renderização
	do ponto de vista da luz obtém o valor 1 - *ou qualquer valor que possa significar "vísivel" - , pois está ocluido por um ponto vísivel, e logo, 
	do ponto de vista da câmera, esse ponto retorna um valor < que o ponto vísivel que o oclui.

	Fenômenos
	- Como a resolução de ambos framebufer - mapa de sombra e normal - potêncialmente são diferentes, no sombreador de fragmento para renderização normal
	um elemento do mapa de sombra será testado com mais de 1 ponto no framebuffer padrão, gerando sombras perceptivelmente serrilhadas.
*/