#include "LayerDepthRenderable.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/Renderer.h"
#include "Graphic/Texture.h"
#include "Base/Values/Numbers/NumberUtils.h"

ska::LayerDepthRenderable::LayerDepthRenderable(Vector2<TileAnimation*> blocks, const Vector2<int>& bases, const Texture& tileset, const unsigned int blockSize, const int flatPriority, const int basePriority, const Point<int> origin) :
	m_blockSize(blockSize) {
	const auto width = blocks.lineSize();
	const auto height = width == 0 ? 0 : blocks.size() / width;
	auto flat = Vector2<TileAnimation*>{};
	flat.reserve(width, height);
	for (auto y = 0u; y < height; y++) {
		for (auto x = 0u; x < width; x++) {
			const auto base = bases.has(x, y) ? bases[x][y] : -1;
			flat.push_back(base < 0 ? blocks[x][y] : nullptr);
		}
	}
	m_flat = std::make_unique<LayerMonoRenderable>(std::move(flat), tileset, blockSize, origin);
	m_flat->setPriority(flatPriority);

	// A row per base row (an object may stand under the bottom of the map : its rows are there too)
	auto lowest = height;
	for (auto y = 0u; y < height; y++) {
		for (auto x = 0u; x < width; x++) {
			if (bases.has(x, y) && bases[x][y] >= 0 && blocks[x][y] != nullptr) {
				lowest = std::max<std::size_t>(lowest, y + static_cast<std::size_t>(bases[x][y]) + 1);
			}
		}
	}
	m_rows.reserve(lowest);
	for (auto row = 0u; row < lowest; row++) {
		m_rows.emplace_back(tileset, blockSize);
		m_rows.back().setPriority(basePriority + origin.y + static_cast<int>(row * blockSize));
	}
	for (auto y = 0u; y < height; y++) {
		for (auto x = 0u; x < width; x++) {
			if (bases.has(x, y) && bases[x][y] >= 0 && blocks[x][y] != nullptr) {
				m_rows[y + static_cast<std::size_t>(bases[x][y])].add({ static_cast<int>(x) + origin.x / static_cast<int>(blockSize),
					static_cast<int>(y) + origin.y / static_cast<int>(blockSize) }, blocks[x][y]);
			}
		}
	}
}

void ska::LayerDepthRenderable::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	m_flat->graphicUpdate(cameraPos, drawables);
	for (auto& row : m_rows) {
		if (!row.empty()) {
			row.setCamera(cameraPos);
			drawables.add(row);
		}
	}
}

void ska::LayerDepthRenderable::BaseRow::render(Renderer& renderer) const {
	// Signed : a tile partly out of the left or top of the screen is at a negative position (NumberUtils::absolute gives an
	// unsigned int : the subtraction wrapped around, the tile was culled, the blocks along those edges were left black)
	const auto cameraX = static_cast<int>(NumberUtils::absolute(m_camera.x));
	const auto cameraY = static_cast<int>(NumberUtils::absolute(m_camera.y));
	const auto size = static_cast<int>(m_blockSize);
	for (const auto& [block, animation] : m_tiles) {
		const auto x = block.x * size - cameraX;
		const auto y = block.y * size - cameraY;
		if (x + size < 0 || y + size < 0 || x > m_camera.w || y > m_camera.h) {
			continue;
		}
		const auto* clip = animation->animated ? &animation->animation.getCurrentFrame() : &animation->animation.getOffsetBase();
		renderer.render(*m_tileset, x, y, clip, 0, nullptr);
	}
}
