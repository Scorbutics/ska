#include <fstream>
#include "World/__internalConfig/LoggerConfig.h"
#include "LightMap.h"
#include "Graphic/Texture.h"
#include "Graphic/SDLSurface.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/Renderer.h"

ska::LightMap::LightMap() = default;
ska::LightMap::~LightMap() = default;

void ska::LightMap::load(const std::string& file, unsigned int blockSize, unsigned int blocksX, unsigned int blocksY, const Point<int> origin) {
	if (file == m_file) {
		return;
	}
	m_file = file;
	m_texture = nullptr;
	if (file.empty() || !std::ifstream{ file }) {
		return;
	}
	// The colour key of the textures (a purple) is never a light : no pixel of a light map disappears
	m_texture = std::make_unique<Texture>(file);
	prepare(blockSize, blocksX, blocksY, origin);
}

void ska::LightMap::load(const std::string& file, SDLSurface&& image, unsigned int blockSize, unsigned int blocksX, unsigned int blocksY, const Point<int> origin) {
	m_file = file;
	m_texture = std::make_unique<Texture>();
	m_texture->load(file, std::move(image));
	prepare(blockSize, blocksX, blocksY, origin);
}

void ska::LightMap::prepare(unsigned int blockSize, unsigned int blocksX, unsigned int blocksY, const Point<int> origin) {
	m_blockSize = blockSize;
	m_blocksX = blocksX;
	m_blocksY = blocksY;
	m_origin = origin;
	// Multiplied with the drawing under it
	m_texture->setBlendMode(SDL_BLENDMODE_MOD);
	SLOG(LogLevel::Info) << "Light map " << m_file << " : " << blocksX << " x " << blocksY << " blocks";
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
	// Pixel i of the image is the corner i of the blocks : the centre of each pixel lands on its corner, the blocks of the
	// map go from the centre of the first pixel to the centre of the last one
	const auto destination = Rectangle{ m_origin.x - m_cameraPos.x, m_origin.y - m_cameraPos.y,
		static_cast<int>(m_blocksX * m_blockSize), static_cast<int>(m_blocksY * m_blockSize) };
	renderer.renderRegion(*m_texture, 0.5F, 0.5F, static_cast<float>(m_blocksX), static_cast<float>(m_blocksY), destination);
}
