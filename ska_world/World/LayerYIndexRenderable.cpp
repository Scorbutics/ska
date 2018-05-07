#include "LayerYIndexRenderable.h"

ska::LayerYIndexRenderable::LayerYIndexRenderable(Vector2<TileAnimation*> block, const Texture& tileset, unsigned int blockSize, unsigned int priority) :
	m_blockSize(blockSize),
	m_tileset(tileset) {

	const auto width = block.lineSize();
	const auto height = width == 0 ? 0 : (block.size() / block.lineSize());

	if (height != 0) {
		m_lines.reserve(height);
		for(auto i = 0u; i < height; i++) {
			auto row = LayerRenderableRow{};
			row.load(i, block, m_tileset, blockSize);
			row.setPriority(priority + i * blockSize);
			m_lines.push_back(std::move(row));
		}
	}
}

void ska::LayerYIndexRenderable::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	const auto absORelY = NumberUtils::absolute(cameraPos.y);
	const auto cameraPositionStartBlockY = absORelY / m_blockSize;
	auto cameraPositionEndBlockY = (absORelY + cameraPos.w) / m_blockSize;

	if(cameraPositionStartBlockY > m_lines.size()) {
		return;
	}

	if (cameraPositionEndBlockY >= m_lines.size()) {
		cameraPositionEndBlockY = m_lines.size() - 1;
	}

	for (auto i = cameraPositionStartBlockY; i <= cameraPositionEndBlockY; i++) {
		m_lines[i].graphicUpdate(cameraPos, drawables);
	}
	
}

void ska::LayerYIndexRenderable::setPriority(unsigned int priority) {
	auto i = 0u;
	for (auto& line : m_lines) {
		line.setPriority(priority + i * m_blockSize);
		i++;
	}
}
