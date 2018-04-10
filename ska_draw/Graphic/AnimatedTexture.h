#pragma once
#include "Animation.h"
#include "Texture.h"
#include "Rectangle.h"
#include "Point.h"
#include "GifTexture.h"
#include "Draw/Renderable.h"

namespace ska {
	struct Color;

	class AnimatedTexture : public Renderable {
		friend class SDLRenderer;
	public:
		AnimatedTexture();
		~AnimatedTexture() = default;

		void loadFromText(unsigned fontSize, const std::string& text, Color c, unsigned horizontalFrames, unsigned verticalFrames, unsigned animatedFrames, bool isVertical = false);
		void load(const std::string& spritePath, unsigned int horizontalFrames = 1, unsigned int verticalFrames = 1, unsigned int animatedFrames = 1, bool isVertical = false, int r = Texture::DEFAULT_T_RED, int g = Texture::DEFAULT_T_GREEN, int b = Texture::DEFAULT_T_BLUE, int a = -1);

		void stop(bool x);
		void reset();
		void nextFrame();

		void setDelay(unsigned int delay);
		void update();
		void setAlpha(int alpha);
		void setColor(int red, int green, int blue);
		void setOffset(const Point<int>& offset);
		void setRelativePosition(const Point<int>& p);

		unsigned int getWidth() const;
		unsigned int getHeight() const;
		Rectangle getOffsetBase() const;


		unsigned int getFullWidth() const;
		unsigned int getFullHeight() const;

		void operator=(const AnimatedTexture& text);
		void switchToFrame(unsigned int index);
		void setBlendMode(int blendMode);
		void lifetimeSeparation();
	private:
		void refresh() const;
		void recalculateFrames(unsigned int horizontalFrames, unsigned int verticalFrames, unsigned int animatedFrames, bool isVertical);

	public:
		void render(const Renderer& renderer, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const override;

	private:
		Point<int> m_relativePos;
		Animation m_anim;

		bool m_gifMode;
		GifTexture m_gif;
		Texture m_sprite;
	};
}

