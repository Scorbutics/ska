#pragma once
#include "Tile.h"
#include "Layer.h"
#include "Utils/MovableNonCopyable.h"
#include "Utils/TupleUtils.h"
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

		bool collide(std::size_t layerMax, std::size_t blockX, std::size_t blockY) const;
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
		
		std::vector<LayerPtr> m_layers;

		std::size_t m_blocksX{};
		std::size_t m_blocksY{};
		unsigned int m_blockSize;
	};
}
