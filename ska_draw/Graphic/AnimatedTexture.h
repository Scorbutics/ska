#pragma once
#include "Animation.h"
#include "Texture.h"
#include "Rectangle.h"
#include "Point.h"
#include "GifTexture.h"

namespace ska {
	class AnimatedTexture :
		public Renderable {
	public:
		AnimatedTexture();
		~AnimatedTexture() = default;

		void loadFromText(unsigned fontSize, const std::string& text, Color c, unsigned horizontalFrames, unsigned verticalFrames, unsigned animatedFrames, bool isVertical = false);
		void load(const std::string& spritePath, unsigned int horizontalFrames = 1, unsigned int verticalFrames = 1, unsigned int animatedFrames = 1, bool isVertical = false, int r = DEFAULT_T_RED, int g = DEFAULT_T_GREEN, int b = DEFAULT_T_BLUE, int a = -1);

		void stop(bool x);
		void reset();
		void nextFrame();

		int render(int x, int y, const Rectangle* clip = nullptr) const override;

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

		void recalculateFrames(unsigned int horizontalFrames, unsigned int verticalFrames, unsigned int animatedFrames, bool isVertical);

		Point<int> m_relativePos;
		Animation m_anim;

		bool m_gifMode;
		GifTexture m_gif;
		Texture m_sprite;
	};
}

