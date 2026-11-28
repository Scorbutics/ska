#pragma once
#include "TilesetLoader.h"
#include "Graphic/SDLSurface.h"
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"

namespace ska {
	class TilesetLoaderImage : public TilesetLoader {
	public:
		TilesetLoaderImage(std::string tilesetName);
		// From the images already read : the tileset, its collisions, its properties, its masks (an empty surface : none).
		// The tileset image goes to the texture (loadGraphics, once)
		TilesetLoaderImage(std::string tilesetName, SDLSurface chipset, SDLSurface collisions, SDLSurface properties, SDLSurface mask);
		TilesetLoaderImage(TilesetLoaderImage&&) = default;

		Vector2<Tile> loadPhysics() const override;
		Texture loadGraphics() const override;
		Vector2<TileAnimation> loadAnimations(unsigned int blockSize) const override;

		std::string getName() const override;

	private:
		void checkSizes() const;

		const std::string m_tilesetName;

		// Given to the texture by loadGraphics when it was read by the caller
		mutable SDLSurface m_sChipset;
		const SDLSurface m_sCol;
		const SDLSurface m_sProperties;
		// The collision masks (<tileset>.png.mask, Tile::MaskCells pixels a tile side, white a solid cell) : optional, else
		// a blocking tile is solid all over
		const SDLSurface m_sMask;

	};
}
