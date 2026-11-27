#pragma once
#include "Tile.h"
#include "Layer.h"
#include "Base/Values/MovableNonCopyable.h"
#include "Base/Meta/TupleUtils.h"
#include "MarchingSquare.h"

namespace ska {
	class CollisionProfile : public MovableNonCopyable {
	using LayerPtr = std::unique_ptr<Layer>;
	public:
		explicit CollisionProfile(unsigned int blockSize);
		CollisionProfile(unsigned int blockSize, std::vector<LayerPtr> layers);
		
		CollisionProfile(CollisionProfile&&) = default;
		CollisionProfile& operator=(CollisionProfile&&) = default;
		~CollisionProfile() = default;

		unsigned int getBlockSize() const;
		std::size_t getBlocksX() const;
		std::size_t getBlocksY() const;
		
		bool empty() const;
		std::size_t layers() const;

		// The block cannot be stood on : the cells under a character (StandCells : the middle of its lower half) are solid
		bool collide(std::size_t layerMax, std::size_t blockX, std::size_t blockY) const;
		// The solid cells of a block (Tile::mask) : the union of the colliding tiles of the layers up to layerMax (out of the
		// map : all)
		std::uint16_t solidCells(std::size_t layerMax, std::size_t blockX, std::size_t blockY) const;
		// A point (pixels) is in a solid cell
		bool solidAt(std::size_t layerMax, int x, int y) const;
		// The cells where a character stands on its block (Tile::MaskCells x Tile::MaskCells) : the middle columns of the lower
		// half
		static constexpr std::uint16_t StandCells = (1u << 9) | (1u << 10) | (1u << 13) | (1u << 14);
		Rectangle placeOnNearestPracticableBlock(std::size_t layerMax, const Rectangle& hitBox, unsigned int radius) const;
		Point<int> alignOnBlock(const Rectangle& hitbox) const;
		
		const Tile* getBlock(std::size_t layer, std::size_t blockX, std::size_t blockY) const;
		
		const Tile* getHighestCollidingBlock(std::size_t layerTop, std::size_t blockX, std::size_t blockY) const;
		const Tile* getHighestNonCollidingBlock(std::size_t layerTop, std::size_t blockX, std::size_t blockY) const;
		const Tile* getHighestBlockOnPredicate(const std::size_t layerTop, const std::size_t blockX, const std::size_t blockY, const MarchingSquarePredicate& pred) const;

	private:
		std::pair<std::size_t, std::size_t> safeGetSizes() const;
		void calculate();

		std::vector<Vector2<TileCollision>> m_collisions;
		std::vector<Vector2<std::uint16_t>> m_cells;
		
		std::vector<LayerPtr> m_layers;

		std::size_t m_blocksX{};
		std::size_t m_blocksY{};
		unsigned int m_blockSize;
	};
}
