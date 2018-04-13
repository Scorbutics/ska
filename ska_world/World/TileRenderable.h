#pragma once

#include <memory>
#include "Graphic/Animation.h"
#include "Point.h"

namespace ska {

	class TileRenderable {
	public:
		TileRenderable() = default;
		TileRenderable(unsigned int blockSize, Point<int> posChipset, bool autoAnimation);
		~TileRenderable() = default;

		void refresh() const;
		Rectangle determineFrame(const Rectangle* rectAnim = nullptr) const;
		void setSpriteFrame(unsigned int x);

	private:
		mutable Animation m_anim {};

		bool m_autoAnimation = false;
		Rectangle m_spritePosInChipset{};

	};

	typedef std::unique_ptr<TileRenderable> BlockRenderablePtr;
}
