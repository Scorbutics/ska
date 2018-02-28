#pragma once

#include <memory>

#include "BlockRenderable.h"
#include "Draw/DrawableFixedPriority.h"
#include "Utils/Vector2.h"

class ParticleManager;

namespace ska {
	class World;
	class LayerRenderable : public DrawableFixedPriority {
	public:
		LayerRenderable(const unsigned int blockSize);
		void operator=(const LayerRenderable&) = delete;
		~LayerRenderable() = default;

		void reset(Vector2<BlockRenderable*>&& block);

		void update();
		void clear();

		void render(const Renderer& renderer) const override;
		bool isVisible() const override;

		BlockRenderable* getBlock(unsigned int i, unsigned int j);

	private:
		const unsigned int m_blockSize;
		World& m_world;
		Vector2<BlockRenderable*> m_block;

	};
	typedef std::unique_ptr<LayerRenderable> LayerRenderablePtr;


}

