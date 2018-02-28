#pragma once

#include "Physic/Block.h"
#include "Utils/Vector2.h"

namespace ska {
	class Layer {
	public:
		Layer();
		void operator=(const Layer&) = delete;
		~Layer() = default;

		BlockCollision getCollision(const unsigned int i, const unsigned int j) const;
		
		void reset(ska::Vector2<ska::Block*>&& block);

		unsigned int getBlocksX() const;
		unsigned int getBlocksY() const;
		Block const* getBlock(const unsigned x, const unsigned y) const;

	private:		
		Vector2<Block*> m_block;
		unsigned int m_fileWidth;
		unsigned int m_fileHeight;

	};

}

