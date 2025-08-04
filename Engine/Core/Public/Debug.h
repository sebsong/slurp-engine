#pragma once

#if DEBUG
#define assert(expression) if (!(expression)) *(int*)(nullptr) = 0
#else
#define assert(expression) 
#endif

#define VERBOSE 0

#define DEBUG_DRAW_COLLISION 0
#define DEBUG_RED_COLOR 0xFFFF0000
#define DEBUG_GREEN_COLOR 0xFF00FF00
#define DEBUG_BLUE_COLOR 0xFF0000FF
#define DEBUG_DRAW_COLOR DEBUG_GREEN_COLOR

#define DEBUG_SHOW_MOUSE_CURSOR 0
