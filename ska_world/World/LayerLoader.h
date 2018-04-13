#pragma once
#include "Physic/Tile.h"
#include "Graphic/SDLSurface.h"
#include "Utils/Vector2.h"
#include "TileRenderable.h"
#include "TilesetCorrespondanceMapper.h"

namespace ska {
	class Tileset;

	class LayerLoader : public MovableNonCopyable {
	public :
		virtual Vector2<Tile*> loadPhysics(Tileset& chipset) const = 0;
		virtual Vector2<const TileRenderable*> loadGraphics(Tileset& chipset) const = 0;
		~LayerLoader() override = default;
	};

	class LayerLoaderImage : public LayerLoader {
	public:
		static SDLSurface loadFrom32(const std::string& layerFilename);

		LayerLoaderImage(LayerLoaderImage&&) = default;
		explicit LayerLoaderImage(const TilesetCorrespondanceMapper& correspondanceMapper, const std::string& layerFilename);
		
		Vector2<Tile*> loadPhysics(Tileset& chipset) const override;
		Vector2<const TileRenderable*> loadGraphics(Tileset& chipset) const override;
	
	private:
		const TilesetCorrespondanceMapper& m_colorMapper;

		const SDLSurface m_file;
		const int m_fileWidth;
		const int m_fileHeight;
	};
}
