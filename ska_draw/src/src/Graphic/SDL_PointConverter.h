#pragma once
#include <SDL.h>
#include "Core/Point.h"

template <class T>
SDL_Point ToSDL_Point(const ska::Point<T>& p) {
	return SDL_Point{ p.x, p.y };
}
