cl /Foobj/ /EHsc /std:c++17 /I../../../include debug.cpp ../../Common/Core.cpp ../../SRW/SRW.cpp ../Camera.cpp glew32.lib OpenGL32.lib SDL2.lib /link /LIBPATH:../../../lib64