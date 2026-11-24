#include <fstream>
#include "World/__internalConfig/LoggerConfig.h"
#include "LightMap.h"
#include "Graphic/Texture.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/Renderer.h"

ska::LightMap::LightMap() = default;
ska::LightMap::~LightMap() = default;

void ska::LightMap::load(const std::string& file, unsigned int blockSize, unsigned int blocksX, unsigned int blocksY) {
	if (file == m_file) {
		return;
	}
	m_file = file;
	m_blockSize = blockSize;
	m_texture = nullptr;
	if (file.empty() || !std::ifstream{ file }) {
		return;
	}
	// The colour key of the textures (a purple) is never a light : no pixel of a light map disappears
	m_texture = std::make_unique<Texture>(file);
	// The size comes from the map, not from the texture : the textures are shared by file name, and this one is
	// stretched (its size is the size it is drawn at)
	const auto cornersX = blocksX + 1;
	const auto cornersY = blocksY + 1;
	// Multiplied with the drawing under it
	m_texture->setBlendMode(SDL_BLENDMODE_MOD);
	// Pixel i is the corner i of the blocks : the centre of each pixel lands on its corner, so the image spans one block
	// more than the map, half a block out on each side
	m_texture->resize(cornersX * blockSize, cornersY * blockSize);
	SLOG(LogLevel::Info) << "Light map " << file << " : " << cornersX - 1 << " x " << cornersY - 1 << " blocks";
}

bool ska::LightMap::loaded() const {
	return m_texture != nullptr;
}

const std::string& ska::LightMap::file() const {
	return m_file;
}

void ska::LightMap::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	if (m_texture == nullptr) {
		return;
	}
	m_cameraPos = cameraPos;
	drawables.add(*this);
}

void ska::LightMap::render(Renderer& renderer) const {
	if (m_texture == nullptr) {
		return;
	}
	const auto half = static_cast<int>(m_blockSize / 2);
	renderer.render(*m_texture, -m_cameraPos.x - half, -m_cameraPos.y - half, nullptr, 0, nullptr);
}
