#pragma once

#include <memory>
#include "Draw/DrawableFixedPriority.h"
#include "Utils/Vector2.h"
#include "Graphic/Texture.h"

namespace ska {
	class Texture;
	class TileWorld;
	struct TileAnimation;

	class LayerRenderable : public DrawableFixedPriority {
	public:
		LayerRenderable(Vector2<TileAnimation*> block, const Texture& tileset, unsigned int blockSize);
		void operator=(const LayerRenderable&) = delete;
		~LayerRenderable() = default;

		void update(const Rectangle& cameraPos);
		void clear();

		void render(const Renderer& renderer) const override;
		bool isVisible() const override;

	private:
		const unsigned int m_tileSize;

		Vector2<TileAnimation*> m_animations;
		Texture m_tileset;

		Rectangle m_lastCameraPos {};
		unsigned int m_width {};
		unsigned int m_height {};

	};
	using LayerRenderablePtr = std::unique_ptr<LayerRenderable>;

}

