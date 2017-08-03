#pragma once
#include <string>
#include "Rectangle.h"

struct CEV_GifAnim;

namespace ska {
	class SDLRenderer;

	class GifTexture {
	public:
		GifTexture();
		GifTexture(const GifTexture& t);
		~GifTexture();

		GifTexture& operator=(const GifTexture& t);

		void load(const std::string& spriteName);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		int render(int x, int y, const Rectangle* clip = nullptr) const;
		void reset() const;
		void nextFrame() const;
		void setDelay(unsigned delay) const;
		void stop(bool x) const;

	private:
		void setLoopMode(unsigned int loopMode) const;
		void free();

		CEV_GifAnim* m_animation;
		SDL_Texture *m_actTexture;

		unsigned int m_width;
		unsigned int m_height;

        std::string m_spriteName;

		static void checkRenderer();
		static SDLRenderer* m_renderer;
	};
}
