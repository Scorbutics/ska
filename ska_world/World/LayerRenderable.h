#pragma once

#include <memory>
#include <optional>
#include "TileRenderable.h"
#include "Draw/DrawableFixedPriority.h"
#include "Utils/Vector2.h"
#include "TilesetRenderable.h"

namespace ska {
	class TileWorld;
	class LayerRenderable : public DrawableFixedPriority {
	public:
		LayerRenderable(Vector2<std::optional<const TileRenderable>> block, const TilesetRenderable& chipset, unsigned int blockSize);
		void operator=(const LayerRenderable&) = delete;
		~LayerRenderable() = default;

		void update(const ska::Rectangle& cameraPos);
		void clear();

		void render(const Renderer& renderer) const override;
		bool isVisible() const override;

		const TileRenderable* getBlock(unsigned int i, unsigned int j);

	private:

		const unsigned int m_blockSize;
		Vector2<std::optional<const TileRenderable>> m_block;
		const TilesetRenderable& m_tileset;
		ska::Rectangle m_lastCameraPos;
		unsigned int m_width{};
		unsigned int m_height{};

	};
	using LayerRenderablePtr = std::unique_ptr<LayerRenderable>;

}

