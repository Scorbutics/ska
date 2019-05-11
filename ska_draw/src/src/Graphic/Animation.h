#pragma once
#include "Core/Rectangle.h"

namespace ska {
	class Animation {
		friend class AnimatedTexture;
	public:
		Animation();
		Animation(unsigned int delay, int frameNumber, bool isVertical, int x = 0, int y = 0, unsigned int width = 0, unsigned int height = 0);
		void stop(bool x);
		void nextFrame();

		const Rectangle& getCurrentFrame() const;
		const Rectangle& updateFrame();

		void switchToFrame(unsigned int number);
		const Rectangle& getOffsetBase() const;

	private:
		void setVertical(const bool b);
		void setOffsetAndFrameSize(Rectangle s);
		void setDelay(unsigned int x);
		void setFrames(const unsigned int framesNumber);

		//TODO simplifier
		Rectangle m_offsetBase; 
		Rectangle m_offset;
		int m_position;
		int m_sensAni;
		int m_frameNumber;
		unsigned int m_count;
		unsigned int m_countMAX; 
		unsigned int m_cycles;
		bool m_isVertical;
		bool m_active;
	};
}

