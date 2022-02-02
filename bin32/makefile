OBJS = ../libraries/GUI/GUI.cpp ../libraries/GUI/Panel.cpp  ../libraries/CommandLine/CommandLine.cpp ../libraries/Camera/Camera.cpp ../libraries/Text/Text.cpp ../libraries/Quad/Quad.cpp ../libraries/Terrain/Terrain.cpp ../libraries/Object/Object.cpp ../libraries/SRW/SRW.cpp main.cpp 
OBJ_NAME = main.exe
CC = g++
INCLUDE_PATHS = -I../include/SDL2 -I../include/OpenGL -I../include/freetype -I../include
LIBRARY_PATHS = -L../lib32 -L../lib/freetype
LINKER_FLAGS  = -std=c++17 -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -lfreetype
COMPILE_FLAGS = -W -isystem -mwindows

all: $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILE_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# -W : warnings
#-isystem : console output