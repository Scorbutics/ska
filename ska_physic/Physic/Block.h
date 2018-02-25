#pragma once

#include <memory>
#include "Point.h"

namespace ska {

	class Block {
	public:
		Block(int corrFileWidth, Point<int> posChipset, unsigned int properties, int col);
		~Block() = default;

		unsigned int getProperties() const;
		int getCollision() const;
		unsigned int getID() const;

	private:
		unsigned int m_properties;
		unsigned int m_id;
		int m_collision;

	};

	using BlockPtr = std::unique_ptr<Block>;
}
