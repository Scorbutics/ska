#include "LayerYIndexRenderable.h"

ska::LayerYIndexRenderable::LayerYIndexRenderable(Vector2<TileAnimation*> block, const Texture& tileset, unsigned int blockSize, unsigned int priority) :
	m_blockSize(blockSize),
	m_tileset(tileset) {

	const auto width = block.lineSize();
	const auto height = width == 0 ? 0 : (block.size() / block.lineSize()) * m_blockSize;

	if (height != 0) {
		m_lines.reserve(height);
		for(auto i = 0u; i < width; i++) {
			auto row = LayerRenderableRow{};
			row.load(i, block, m_tileset, blockSize);
			row.setPriority(priority + i * blockSize);
			m_lines.push_back(std::move(row));
		}
	}
}

void ska::LayerYIndexRenderable::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	for(auto& line : m_lines) {
		line.graphicUpdate(cameraPos, drawables);
	}
}

void ska::LayerYIndexRenderable::setPriority(unsigned int priority) {
	auto i = 0u;
	for (auto& line : m_lines) {
		line.setPriority(priority + i * m_blockSize);
		i++;
	}
}
