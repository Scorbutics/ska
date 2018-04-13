#pragma once

#include "Physic/Tile.h"
#include "Utils/Vector2.h"

namespace ska {
	class Layer {
	public:
		Layer(ska::Vector2<ska::Tile*>&& block);
		void operator=(const Layer&) = delete;
		~Layer() = default;

		TileCollision getCollision(const unsigned int i, const unsigned int j) const;

		unsigned int getBlocksX() const;
		unsigned int getBlocksY() const;
		Tile const* getBlock(const unsigned x, const unsigned y) const;

	private:		
		void reset(ska::Vector2<ska::Tile*>&& block);
		Vector2<Tile*> m_block;
		unsigned int m_fileWidth;
		unsigned int m_fileHeight;

	};

}

