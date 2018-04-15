#pragma once

#include "Physic/Tile.h"
#include "Utils/Vector2.h"

namespace ska {
	class Layer {
	public:
		Layer(Vector2<Tile*>&& block);
		void operator=(const Layer&) = delete;
		~Layer() = default;

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

