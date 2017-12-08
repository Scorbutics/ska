#pragma once

#include "Utils/ManagedResource.h"
#include "Draw/SDLTexture.h"
#include "Draw/TextureData.h"
#include "Utils/SkaConstants.h"
#include "Rectangle.h"
#include "Draw/Color.h"

namespace ska {
	class SDLRenderer;
	class Texture : 
		public ManagedResource<SDLTexture, TextureData> {
		
		friend class Renderer;

	public:
		explicit Texture(const std::string& id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		Texture() = default;
		void loadFromText(unsigned int fontSize, std::string text, Color c);
		void loadFromColoredRect(unsigned int width, unsigned int height, Color c);
		void load(const std::string& id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		virtual ~Texture() = default;

		void setColor(Uint8 red, Uint8 green, Uint8 blue) const;
		void setBlendMode(int blending) const;
		void setAlpha(Uint8 alpha) const;
		void resize(unsigned int width, unsigned int height);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		SDLTexture* getInstance() const {
			return m_value.get();
		}

	};
}



