#ifndef CORE_H
#define CORE_H

// - GLEW/GL
#include "GL/glew.h"

// - SDL
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

// - Standart C++ headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Core{
public:
    virtual void _baseclass_() = 0;
    virtual ~Core();
    static bool initialize(std::string title, int w, int h, bool border = true, bool fullscreen = false);
    static void shutdown();
    static SDL_Window* Window;
    static SDL_GLContext GLContext;
    static int ScreenWidth;
    static int ScreenHeight;
    static bool Quit;
    static std::ofstream LogOutput;    
};
#endif