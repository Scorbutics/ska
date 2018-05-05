#pragma once
#include <memory>
#include <optional>
#include "Tile.h"
#include "Utils/Vector2.h"
#include "Utils/MovableNonCopyable.h"

namespace ska {
	class Layer :
        public MovableNonCopyable {
	public:
		explicit Layer(Vector2<std::optional<Tile>> block, bool top = false);
		Layer(Layer&&) = default;
		~Layer() override = default;

		TileCollision getCollision(std::size_t x, std::size_t y) const;

		unsigned int getBlocksX() const;
		unsigned int getBlocksY() const;
		const Tile* getBlock(std::size_t x, std::size_t y) const;

		bool isTop() const;

	private:
		void reset(Vector2<std::optional<Tile>> block);

		Vector2<std::optional<Tile>> m_block;
		unsigned int m_fileWidth;
		unsigned int m_fileHeight;
		const bool m_top;
	};

	using LayerPtr = std::unique_ptr<Layer>;
}

