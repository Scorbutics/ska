#pragma once

#include "Utils/ManagedResource.h"
#include "Rectangle.h"
#include "Draw/Renderable.h"
#include "SDLTexture.h"
#include "TextureData.h"

namespace ska {
	class Renderer;
	class Texture :
		public ManagedResource<SDLTexture, TextureData>,
		public Renderable {

		friend class SDLRenderer;

	public:
		static constexpr auto DEFAULT_T_RED = 190;
		static constexpr auto DEFAULT_T_GREEN = 151;
		static constexpr auto DEFAULT_T_BLUE = 255;

		Texture() = default;
		explicit Texture(const std::string& id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		
		void loadFromText(unsigned int fontSize, std::string text, Color c);
		void loadFromColoredRect(unsigned int width, unsigned int height, Color c);
		void load(const std::string& id, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);
		virtual ~Texture() = default;

		void setColor(Uint8 red, Uint8 green, Uint8 blue);
		void setBlendMode(int blending);
		void setAlpha(Uint8 alpha);
		void resize(unsigned int width, unsigned int height);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		SDLTexture* getInstance() const;

	public:
		void render(const Renderer& renderer, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const override;
	};
}
