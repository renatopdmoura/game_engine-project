OBJS = ../source/SRW.cpp ../source/Terrain.cpp ../source/Light.cpp ../source/Scene.cpp ../source/Material.cpp ../source/main.cpp 
OBJ_NAME = main_m3D
CC = g++
INCLUDE_PATHS = -I../include/SQLite3 -I../include/SDL2 -I../include/OpenGL -I../include/glm -I../include/glm/gtc -I../include/freetype -I../include
LIBRARY_PATHS = -L../lib -L../lib/freetype
LINKER_FLAGS  = -std=c++17 -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -lfreetype -lsqlite3
COMPILE_FLAGS = -W -isystem -mwindows

all: $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILE_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# -W : warnings
#-isystem : console output