#pragma once

#include <memory>
#include "Point.h"

namespace ska {
	enum class BlockCollision {
		YES,
		NO,
		VOID
	};

	class Block {
	public:
		Block(int corrFileWidth, Point<int> posChipset, unsigned int properties, BlockCollision col);
		~Block() = default;

		unsigned int getProperties() const;
		BlockCollision getCollision() const;
		unsigned int getID() const;

	private:
		unsigned int m_properties;
		unsigned int m_id;
		BlockCollision m_collision;

	};

}
