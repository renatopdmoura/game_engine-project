#include "Core.hpp"

SDL_Window* Core::Window = NULL;
SDL_GLContext Core::GLContext;
int Core::ScreenWidth    = 800;
int Core::ScreenHeight   = 800;
bool Core::Quit          = false;
std::ofstream Core::LogOutput("log.txt", std::ios_base::trunc | std::ios_base::binary);

Core::~Core(){
	if(Core::Window != NULL){
		Core::shutdown();
		SDL_Log("Core::Info: The application core has been terminated!");
	}
}

bool Core::initialize(std::string title, int w, int h, bool border, bool fullscreen){
	Core::ScreenWidth  = w;
	Core::ScreenHeight = h;
	bool success = true;
	Uint32 flags = 0;
	if(fullscreen)
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE |  SDL_WINDOW_SHOWN;
	else
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |  SDL_WINDOW_SHOWN;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
		SDL_Log("SDL unable initialize! SDL Error: %s", SDL_GetError());
		success = false;
	}
	else{
		// GLint max_combined_texture_image_units;
		// glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_image_units);
		// std::cout << max_combined_texture_image_units << std::endl;
		// GLint max_uniform_locations;
		// glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &max_uniform_locations);
		// std::cout << max_uniform_locations << std::endl;	
		// GLint max_compute_texture_image_units;
		// glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_compute_texture_image_units);
		// std::cout << max_compute_texture_image_units << std::endl;	
		
		// - Define a versão principal e secundária do núcleo OpenGL 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);	
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		
		// - Define o tamanho de determinados buffers
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		// - Define o perfil OpenGL, neste caso, desabilita a versão legada
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		
		// - Cria uma janela OpenGL
		// SDL_DisplayMode DM;
		// SDL_GetCurrentDisplayMode(0, &DM);
		// Core::ScreenWidth  = DM.w;
		// Core::ScreenHeight = DM.h;
		Core::Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Core::ScreenWidth, Core::ScreenHeight, flags);

		if(Core::Window == NULL){
			SDL_Log("Couldn't create window! SDL Error: %s", SDL_GetError());
			success = false;
		}
		else{
			SDL_SetWindowBordered(Core::Window, SDL_bool(border));
			SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
			SDL_SetCursor(cursor);
			SDL_ShowCursor(SDL_TRUE);
			SDL_SetRelativeMouseMode(SDL_TRUE);
			// SDL_SetWindowGrab(window, SDL_TRUE);
			SDL_WarpMouseInWindow(Core::Window, Core::ScreenWidth/ 2, Core::ScreenHeight / 2);
			// - Define o contexto OpenGL
			Core::GLContext = SDL_GL_CreateContext(Core::Window);
			if(GLContext == NULL){
				SDL_Log("Couldn't create context! SDL Error: %s", SDL_GetError());
				success = false;
			}
			else{
				// - Carrega extensões OpenGL ARB, EXT, e.g.
				glewExperimental = GL_TRUE;
				GLenum error = glewInit();
				if(error != GLEW_OK){
					SDL_Log("glewInit failed! %s", glewGetErrorString(error));
					success = false;
				}
				else{
					// - Consulta informações de contexto
					// SDL_Log("%s", glGetString(GL_VERSION));
					Core::LogOutput << "INFO: " << glGetString(GL_VERSION)  << std::endl;
					// SDL_Log("%s", glGetString(GL_VENDOR));
					Core::LogOutput << "INFO: " << glGetString(GL_VENDOR)   << std::endl;
					// SDL_Log("%s", glGetString(GL_RENDERER));
					Core::LogOutput << "INFO: " << glGetString(GL_RENDERER) << std::endl;
					// SDL_Log("Shader Languages Supported %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
					Core::LogOutput << "INFO: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					// SDL_Log("%s", "------Extensions supported------");
					// Core::LogOutput << "INFO:------Extensions supported------" << std::endl; 
					// GLint data;
					// glGetIntegerv(GL_NUM_EXTENSIONS, &data);
					// for(GLint k = 0; k < data; ++k){
					// 	SDL_Log("%s", glGetStringi(GL_EXTENSIONS, k));
					// 	LogOutput << "INFO: " << glGetStringi(GL_EXTENSIONS, k) << std::endl;
					// }
					Core::LogOutput.close();		
				}
				// - Configura a taxa de troca de quadros
				if(SDL_GL_SetSwapInterval(-1) < 0){
					if(SDL_GL_SetSwapInterval(1) < 0){
						SDL_Log("Warning: Unable to set Vsync! SDL Error: %s", SDL_GetError());
						success = false;
					}
				}
			}
		}
	}
	return success;
}

void Core::shutdown(){
	SDL_DestroyWindow(Core::Window);
	Core::Window = NULL;
	SDL_Quit();
}