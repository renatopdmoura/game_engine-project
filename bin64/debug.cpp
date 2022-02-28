// - GLEW/GL
#include <glew.h>

// - SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>

// - Standart C++ headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

SDL_GLContext context;
SDL_Window* window = NULL;

std::ofstream writeOutputInfo("info.txt", std::ios_base::trunc | std::ios_base::binary);

bool initialize(std::string title, int w, int h, bool border = true, bool fullscreen = false){
	// ext_screen_width  = w;
	// ext_screen_height = h;
	bool success = true;
	Uint32 flags;
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
		//Define a versão principal e secundária do núcleo OpenGL 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);	
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		
		//Define o tamanho de determinados buffers
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		//Define o perfil OpenGL, neste caso, desabilita a versão legada
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		
		//Cria uma janela OpenGL
		 /*SDL_DisplayMode DM;
		 SDL_GetCurrentDisplayMode(0, &DM);
		 screenWidth  = DM.w;
		 screenHeight = DM.h;*/
		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags);

		if(window == NULL){
			SDL_Log("Couldn't create window! SDL Error: %s", SDL_GetError());
			success = false;
		}
		else{
			SDL_SetWindowBordered(window, SDL_bool(border));
			SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
			SDL_SetCursor(cursor);
			SDL_ShowCursor(SDL_TRUE);
			//SDL_SetRelativeMouseMode(SDL_TRUE);
			//SDL_SetWindowGrab(window, SDL_TRUE);
			SDL_WarpMouseInWindow(window, w / 2, h / 2);
			//Define o contexto OpenGL
			context = SDL_GL_CreateContext(window);
			if(context == NULL){
				SDL_Log("Couldn't create context! SDL Error: %s", SDL_GetError());
				success = false;
			}
			else{
				//Carrega extensões OpenGL ARB, EXT, e.g.
				glewExperimental = GL_TRUE;
				GLenum error = glewInit();
				if(error != GLEW_OK){
					SDL_Log("glewInit failed! %s", glewGetErrorString(error));
					success = false;
				}
				else{
					//Consulta informações de contexto
					// SDL_Log("%s", glGetString(GL_VERSION));
					writeOutputInfo << "INFO: " << glGetString(GL_VERSION)  << std::endl;
					// SDL_Log("%s", glGetString(GL_VENDOR));
					writeOutputInfo << "INFO: " << glGetString(GL_VENDOR)   << std::endl;
					// SDL_Log("%s", glGetString(GL_RENDERER));
					writeOutputInfo << "INFO: " << glGetString(GL_RENDERER) << std::endl;
					// SDL_Log("Shader Languages Supported %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
					writeOutputInfo << "INFO: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					// SDL_Log("%s", "------Extensions supported------");
					// writeOutputInfo << "INFO:------Extensions supported------" << std::endl; 
					GLint data;
					glGetIntegerv(GL_NUM_EXTENSIONS, &data);
					for(GLint k = 0; k < data; ++k){
						// SDL_Log("%s", glGetStringi(GL_EXTENSIONS, k));
						// writeOutputInfo << "INFO: " << glGetStringi(GL_EXTENSIONS, k) << std::endl;
					}
					// writeOutputInfo.close();		
				}
				//Configura a taxa de troca de quadros
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

void shutdown(){
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
}

std::string readShaderSource(const char* pathfile){
	std::string strShader;
	std::ifstream fromFile(pathfile, std::ios_base::binary);
	if(!fromFile.is_open()){
		SDL_Log("Could not open file %s!", pathfile);
	}
	else{
		std::stringstream shader;
		shader << fromFile.rdbuf();
		strShader = shader.str();
		fromFile.close();
	}
	return strShader;
}

void printProgramLog(GLuint program){
	//Verifica se o parâmetro é um objeto de programa válido
	if(glIsProgram(program)){
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		//Consulta um parâmetro do objeto de programa
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = new char[maxLength];
		//Retorna a informação de log de um objeto de programa
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if(infoLogLength > 0){
			SDL_Log("%s", infoLog);
			writeOutputInfo << infoLog << std::endl;
		}
		delete[] infoLog;
	}
	else{
		SDL_Log("Name %d is not a program!", program);
	}
}

void printShaderLog(GLuint shader){
	if(glIsShader(shader)){
		int infoLogLength = 0;
		int maxLength = infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = new char[maxLength];
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if(infoLogLength > 0){
			SDL_Log("%s", infoLog);
			writeOutputInfo << infoLog << std::endl;
		}
		delete[] infoLog;
	}
	else{
		SDL_Log("Name %d is not a sahder!", shader);
	}
}

bool createShader(GLuint& program, GLenum shaderType, const std::string& shaderSource){
	GLuint shader = glCreateShader(shaderType);
	const char* strFileData = shaderSource.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);
	glCompileShader(shader);
	GLint isCompiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled != GL_TRUE){
		switch(shaderType){
			case GL_VERTEX_SHADER:
				SDL_Log("Unable to compile vertex shader! %d\n", shader);
				break;
			case GL_FRAGMENT_SHADER:
				SDL_Log("Unable to compile fragment shader! %d\n", shader);
				break;
			case GL_GEOMETRY_SHADER:
				SDL_Log("Unable to compile geometry shader! %d\n", shader);
				break;
		}
		writeOutputInfo << "Unable to compile vertex shader! " << shader << std::endl;
		printShaderLog(shader);
		return false;
	}
	else{
		glAttachShader(program, shader);
		return true;
	}
}

bool createProgram(GLuint& program){
	program = glCreateProgram();
	if(program == 0){
		SDL_Log("Program create failed!");
		return false;
	}
	return true;
}

bool linkProgram(GLuint& program){
	glLinkProgram(program);
	GLint isLinked = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if(isLinked != GL_TRUE){
		SDL_Log("Error linking program %d", program);
		printProgramLog(program);
		return false;
	}
	return true;
}

int main(int argv, char** args){
	if(initialize("debug", 800, 800)){	
		std::cout << "Success\n";
	}
	return 0;
}
