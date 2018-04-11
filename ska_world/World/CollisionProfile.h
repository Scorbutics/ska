#pragma once
#include "Physic/Block.h"
#include "Layer.h"

namespace ska {
	class CollisionProfile {
	using LayerPtr = std::unique_ptr<Layer>;
	public:
		CollisionProfile() = default;
		~CollisionProfile() = default;

		Layer& addLayer(LayerPtr l);
		Layer& getLayer(unsigned int index);
		bool collide(unsigned int x, unsigned int y) const;
		Block const* getBlock(unsigned int layer, unsigned x, unsigned y) const;
		void clear();

	private:
		std::pair<unsigned, unsigned> safeGetSizes() const;
		BlockCollision getHigherCollision(unsigned int x, unsigned int y) const;
		void calculate() const;

		mutable ska::Vector2<BlockCollision> m_collisions;
		mutable bool m_mustUpdateCollisions = false;

		std::vector<LayerPtr> m_layers;
	};
}
