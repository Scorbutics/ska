#include "TileRenderable.h"

ska::TileRenderable::TileRenderable(const unsigned int blockSize, const Point<int> posChipset, const bool autoAnimation) : 
	m_anim(375, 4, true, 0, 0, blockSize, blockSize) {
	m_autoAnimation = autoAnimation;

	m_spritePosInChipset.x = posChipset.x * blockSize;
	m_spritePosInChipset.y = posChipset.y * blockSize;
	m_spritePosInChipset.w = blockSize;
	m_spritePosInChipset.h = blockSize;

}

void ska::TileRenderable::setSpriteFrame(const unsigned int x) {
	m_anim.switchToFrame(x);
	m_anim.stop(true);
}

void ska::TileRenderable::refresh() const {
	if (m_autoAnimation) {
		m_anim.updateFrame();
	}
}

ska::Rectangle ska::TileRenderable::determineFrame(const Rectangle* rectAnim) const {
	auto buf = m_spritePosInChipset;

	if (m_autoAnimation) {
		Rectangle bufRectAnim{};

		if (rectAnim != nullptr) {
			bufRectAnim = *rectAnim;
		} else  {
			bufRectAnim = m_anim.getCurrentFrame();
		}

		buf.x += bufRectAnim.x;
		buf.y += bufRectAnim.y;
	}

	return buf;
}
