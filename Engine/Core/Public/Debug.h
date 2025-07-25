#pragma once

#if DEBUG
#define assert(expression) if (!(expression)) *(int*)(nullptr) = 0
#else
#define assert(expression) 
#endif

#define VERBOSE 0

#define DEBUG_DRAW_COLLISION 1
#define DEBUG_DRAW_COLOR 0xFF00FF00

#define DEBUG_SHOW_MOUSE_CURSOR 0
