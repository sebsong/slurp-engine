#pragma once

#if DEBUG
#define assert(expression) if (!(expression)) *(int*)(nullptr) = 0
#else
#define assert(expression) 
#endif

#define VERBOSE 0

#define DEBUG_DRAW_COLLISION 0
#define DEBUG_DRAW_COLOR 0x0000FF00
