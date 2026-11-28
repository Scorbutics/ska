#pragma once
#include <memory>
#include <vector>
#include "LayerRenderable.h"
#include "LayerMonoRenderable.h"
#include "TileAnimation.h"
#include "Base/Containers/Vector2.h"
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/Priorized.h"
#include "Core/Point.h"
#include "Core/Rectangle.h"

namespace ska {
	class Texture;
	class DrawableContainer;

	// A layer of a map drawn by the depth of its tiles (TileProperties::base) : its flat tiles in one drawing under the
	// characters (flatPriority) ; its standing tiles grouped by the block row of their base, each row sorted with the
	// characters at basePriority + its pixel row (an object is drawn over the characters standing behind its base, under
	// the ones in front of it, whatever the height of the object : a lamp, a tree, a building of 5 rows)
	class LayerDepthRenderable :
		public LayerRenderable {
	public:
		// bases : the base of each block of the layer (TileProperties::base, -1 for a flat tile or none)
		// origin : where the layer is in the world (pixels) : the rows are sorted by their row in the world
		LayerDepthRenderable(Vector2<TileAnimation*> blocks, const Vector2<int>& bases, const Texture& tileset, unsigned int blockSize, int flatPriority, int basePriority, Point<int> origin = {});
		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) override;
		~LayerDepthRenderable() override = default;

	private:
		// The standing tiles of one base row
		class BaseRow : public DrawableFixedPriority {
		public:
			BaseRow(const Texture& tileset, unsigned int blockSize) : m_tileset(&tileset), m_blockSize(blockSize) {}
			// block : its position in the world
			void add(Point<int> block, TileAnimation* animation) { m_tiles.push_back({ block, animation }); }
			bool empty() const { return m_tiles.empty(); }
			void setCamera(const Rectangle& camera) { m_camera = camera; }
			void render(Renderer& renderer) const override;
			// Its ground line in the world (its priority : from the top of the camera)
			int ground = 0;
		private:
			const Texture* m_tileset;
			unsigned int m_blockSize;
			Rectangle m_camera{};
			std::vector<std::pair<Point<int>, TileAnimation*>> m_tiles;
		};

		std::unique_ptr<LayerMonoRenderable> m_flat;
		std::vector<BaseRow> m_rows;
		unsigned int m_blockSize;
	};
}
