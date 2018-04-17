#pragma once

#include "Physic/Tile.h"
#include "Utils/Vector2.h"
#include "Utils/MovableNonCopyable.h"

namespace ska {
	class Layer :
        public MovableNonCopyable {
	public:
		Layer(Vector2<Tile*>&& block);
		Layer(Layer&&) = default;
		~Layer() override = default;

		TileCollision getCollision(std::size_t x, std::size_t y) const;

		unsigned int getBlocksX() const;
		unsigned int getBlocksY() const;
		Tile const* getBlock(std::size_t x, std::size_t y) const;

	private:
		void reset(Vector2<Tile*>&& block);
		Vector2<Tile*> m_block;
		unsigned int m_fileWidth;
		unsigned int m_fileHeight;

	};

}

