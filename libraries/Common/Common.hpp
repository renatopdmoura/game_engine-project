#ifndef COMMON_H
#define COMMON_H

#ifndef RENDER_DEBUG_MODE
#define RENDER_DEBUG_MODE 1
#endif

typedef unsigned int uint;
typedef unsigned char uchar;

enum{
	UNIFORM_COLOR   = 0,
	TEXTURIZED      = 1,
	TEXT_PROGRAM    = 2,
	TERRAIN_PROGRAM = 3,
	GUI_PROGRAM     = 4,
	GUI_PANEL_PROG  = 5
};

enum{
	UNIFORM_ARRAY,
	UNIFORM_BLOCK,
	SRGB_PROFILE,
	SRGBA_PROFILE,
	DEFAULT_PROFILE
};
#endif