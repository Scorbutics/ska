#pragma once
#include <SDL.h>
#include "Rectangle.h"

inline SDL_Rect ToSDL_Rect(const ska::Rectangle& r) {
	return SDL_Rect{ r.x, r.y, r.w, r.h };
}
