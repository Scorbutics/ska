#pragma once
#include "Tile.h"
#include "Layer.h"
#include "Utils/MovableNonCopyable.h"

namespace ska {
	class CollisionProfile : public MovableNonCopyable {
	using LayerPtr = std::unique_ptr<Layer>;
	public:
		CollisionProfile() = default;
		CollisionProfile(CollisionProfile&&) = default;
		CollisionProfile& operator=(CollisionProfile&&) = default;
		~CollisionProfile() = default;

		Layer& addLayer(LayerPtr l);
		Layer& getLayer(unsigned int index);
		bool collide(unsigned int x, unsigned int y) const;
		const Tile* getBlock(unsigned int layer, unsigned x, unsigned y) const;
		bool empty() const;
		std::size_t layers() const;
		void clear();

	private:
		std::pair<unsigned, unsigned> safeGetSizes() const;
		TileCollision getHigherCollision(unsigned int x, unsigned int y) const;
		void calculate();

		ska::Vector2<TileCollision> m_collisions;

		std::vector<LayerPtr> m_layers;
	};
}
