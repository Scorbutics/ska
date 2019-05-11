#pragma once
#include <memory>
#include <optional>
#include "Tile.h"
#include "Base/Containers/Vector2.h"
#include "Base/Values/MovableNonCopyable.h"

namespace ska {
	class Layer :
        public MovableNonCopyable {
	public:
		explicit Layer(Vector2<std::optional<Tile>> block);
		Layer(Layer&&) = default;
		~Layer() override = default;

		TileCollision getCollision(std::size_t x, std::size_t y) const;

		unsigned int getBlocksX() const;
		unsigned int getBlocksY() const;
		const Tile* getBlock(std::size_t x, std::size_t y) const;

	private:
		void reset(Vector2<std::optional<Tile>> block);

		Vector2<std::optional<Tile>> m_block;
		unsigned int m_fileWidth;
		unsigned int m_fileHeight;
	};

	using LayerPtr = std::unique_ptr<Layer>;
}

