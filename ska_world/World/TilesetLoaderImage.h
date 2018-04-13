#pragma once
#include "TilesetLoader.h"
#include "Graphic/SDLSurface.h"

namespace ska {
	class TilesetLoaderImage : public TilesetLoader {
	public:
		TilesetLoaderImage(std::string tilesetName);
		TilesetLoaderImage(TilesetLoaderImage&&) = default;

		Vector2<Tile> loadPhysics() const override;
		std::pair<Texture, Vector2<TileRenderable>> loadGraphics(unsigned blockSize) const override;

		std::string getName() const override;

	private:
		void checkSizes() const;

		const std::string m_tilesetName;

		const SDLSurface m_sChipset;
		const SDLSurface m_sCol;
		const SDLSurface m_sProperties;

		const uint32_t m_darkColor{};
		const uint32_t m_whiteColor{};
		const uint32_t m_lightColor{};

	};
}
