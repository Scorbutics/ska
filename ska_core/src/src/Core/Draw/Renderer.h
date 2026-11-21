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

	/**
	 * \brief Changes of a drawing (ex : a sprite hit in a battle) : color, transparency, white flash, scale.
	 * The texture is not changed (the textures are shared) : the renderer applies the effect to one drawing.
	 */
	struct RenderEffect {
		// Multiplies the colors of the texture (255 : unchanged)
		unsigned char red = 255;
		unsigned char green = 255;
		unsigned char blue = 255;
		unsigned char alpha = 255;
		// Brightens the texture to white (0 : none, 255 : the strongest)
		unsigned char flash = 0;
		// Scale of the drawing, around its anchor : the bottom center (a sprite on the ground), or its center
		float scaleX = 1.F;
		float scaleY = 1.F;
		bool anchorBottom = true;
		// Moves the drawing (pixels), ex : a sprite lunging forward
		int offsetX = 0;
		int offsetY = 0;

		bool active() const {
			return red != 255 || green != 255 || blue != 255 || alpha != 255 || flash != 0 || scaleX != 1.F || scaleY != 1.F || offsetX != 0 || offsetY != 0;
		}
	};

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
		
		// The effect applied to the next renderings, until it is reset (nullptr)
		void setEffect(const RenderEffect* effect) const {
			m_effect = effect != nullptr && effect->active() ? effect : nullptr;
		}

		const Color& getDefaultColor() const {
			return m_defaultColor;
		}

		void setDefaultColor(const Color& c) {
			m_defaultColor = c;
		}

	protected:
		mutable const RenderEffect* m_effect = nullptr;

	private:
		Color m_defaultColor;

	};
}
