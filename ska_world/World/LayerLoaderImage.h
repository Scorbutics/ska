#pragma once
#include "Physic/Tile.h"
#include "Graphic/SDLSurface.h"
#include "Utils/Vector2.h"
#include "TilesetCorrespondanceMapper.h"
#include "LayerLoader.h"
#include "Graphic/Animation.h"

namespace ska {
	class Tileset;

	class LayerLoaderImage : public LayerLoader {
	public:
		static SDLSurface loadFrom32(const std::string& layerFilename);

		LayerLoaderImage(LayerLoaderImage&&) = default;
		explicit LayerLoaderImage(const TilesetCorrespondanceMapper& correspondanceMapper, const std::string& layerFilename);

		Vector2<std::optional<Tile>> loadPhysics(Tileset& chipset) const override;
		Vector2<Animation*> loadAnimations(Tileset& chipset) const override;

	private:
		const TilesetCorrespondanceMapper& m_colorMapper;

		const SDLSurface m_file;
		const int m_fileWidth;
		const int m_fileHeight;
	};
}
