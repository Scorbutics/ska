#pragma once

#include "../Rectangle.h"
#include "../Point.h"

namespace ska {
	class AnimatedTexture;
	class SDLTexture;
	class SDLSurface;
	class Texture;
	struct Color;

	class Renderer {
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		virtual void render(const Texture& t, int posX, int posY, Rectangle const * clip = nullptr) const = 0;
		virtual void render(AnimatedTexture& at, int x, int y, Rectangle const * clip = nullptr) const = 0;

		virtual SDL_Texture* createTextureFromSurface(const SDLSurface& surface) const = 0;
		virtual void update() const = 0;
		
		virtual void drawColorPoint(const Color& c, const Point<int>& pos) const = 0;
		virtual void drawColorRect(const Color& c, const Rectangle& r) const = 0;
		virtual void drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const = 0;

	};
}
