#pragma once

#include "../Rectangle.h"
#include "../Point.h"
#include "Color.h"

extern "C" {
	struct SDL_Texture;
	typedef struct SDL_Texture SDL_Texture;
}

namespace ska {
	class AnimatedTexture;
	class SDLTexture;
	class SDLSurface;
	class Texture;

	class Renderer {
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		virtual void load() {}

		virtual void render(const Texture& t, int posX, int posY, Rectangle const* clip, double angle = 0., Point<int> const* rotationCenter = nullptr) const = 0;
		virtual void render(const AnimatedTexture& at, int x, int y, Rectangle const* clip, double angle = 0., Point<int> const* rotationCenter = nullptr) const = 0;

		virtual SDL_Texture* createTextureFromSurface(const SDLSurface& surface) const = 0;
		virtual SDL_Texture* createTextureTarget(unsigned int width, unsigned int height) const = 0;
		
		virtual void setRenderTarget(Texture* t) = 0;
		virtual void setRenderDrawBlendMode(int blendMode) = 0;

		virtual void update() const = 0;
		
		virtual void drawColorPoint(const Color& c, const Point<int>& pos) const = 0;
		virtual void drawColorRect(const Color& c, const Rectangle& r) const = 0;
		virtual void drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const = 0;
		
		const Color& getDefaultColor() const {
			return m_defaultColor;
		}

		void setDefaultColor(const Color& c) {
			m_defaultColor = c;
		}

	private:
		Color m_defaultColor;

	};
}
