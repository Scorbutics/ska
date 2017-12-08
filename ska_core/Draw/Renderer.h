#pragma once
#include "../../ska_draw/Graphic/Texture.h"
#include "../../ska_draw/Graphic/AnimatedTexture.h"

namespace ska {
	class SDLTexture;
	class SDLSurface;
	class Texture;

	class Renderer {
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		void render(const Texture& t, int posX, int posY, Rectangle const * clip = nullptr) const {
			auto instance = t.getInstance();
			if (instance != nullptr) {
				render(*instance, posX, posY, clip);
			}
		}

		void render(AnimatedTexture& at, int x, int y, Rectangle const * clip = nullptr) {
			at.refresh();
			const auto& tmp = at.m_anim.getCurrentFrame();
			
			return at.m_gifMode ? (
				render(at.m_gif, x + at.m_relativePos.x, y + at.m_relativePos.y, &tmp) ):
				render(*at.m_sprite.getInstance(), x + at.m_relativePos.x, y + at.m_relativePos.y, &tmp);
		}

		virtual SDL_Texture* createTextureFromSurface(const SDLSurface& surface) const = 0;

	protected:
		virtual void render(SDLTexture& t, int posX, int posY, Rectangle const * = nullptr) const = 0;
		virtual void render(GifTexture& t, int posX, int posY, Rectangle const * = nullptr) const = 0;
	};
}
