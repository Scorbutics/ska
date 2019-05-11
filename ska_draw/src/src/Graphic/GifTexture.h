#pragma once
#include <string>
#include "Core/Rectangle.h"

struct CEV_GifAnim;
struct SDL_Texture;

namespace ska {
	class SDLRenderer;

	class GifTexture {
		friend class SDLRenderer;
	public:
		GifTexture();
		GifTexture(const GifTexture& t);
		~GifTexture();

		GifTexture& operator=(const GifTexture& t);

		void load(const std::string& spriteName);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		void reset() const;
		void nextFrame() const;
		void setDelay(unsigned delay) const;
		void stop(bool x) const;
		void refresh() const;

	private:
		void setLoopMode(unsigned int loopMode) const;
		void free();

		CEV_GifAnim* m_animation;
		SDL_Texture *m_actTexture;

		unsigned int m_width;
		unsigned int m_height;

        std::string m_spriteName;

	};
}
