#pragma once
#include "Physic/Block.h"
#include "Layer.h"

namespace ska {
	class CollisionProfile {
	using LayerPtr = std::unique_ptr<Layer>;
	public:
		CollisionProfile() = default;
		~CollisionProfile() = default;

		ska::Layer& addLayer(LayerPtr l);
		bool collide(const unsigned int x, const unsigned int y) const;
		ska::Block const* getBlock(const unsigned int layer, const unsigned x, const unsigned y) const;
		void clear();

	private:
		std::pair<unsigned, unsigned> safeGetSizes() const;
		BlockCollision getHigherCollision(const unsigned int x, const unsigned int y) const;
		void calculate() const;

		mutable ska::Vector2<BlockCollision> m_collisions;
		mutable bool m_mustUpdateCollisions = false;

		std::vector<LayerPtr> m_layers;
	};
}
