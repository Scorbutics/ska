#pragma once
#include "TilesetLoader.h"
#include "Graphic/SDLSurface.h"
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"

namespace ska {
	class TilesetLoaderImage : public TilesetLoader {
	public:
		TilesetLoaderImage(std::string tilesetName);
		TilesetLoaderImage(TilesetLoaderImage&&) = default;

		Vector2<Tile> loadPhysics() const override;
		Texture loadGraphics() const override;
		Vector2<std::optional<Animation>> loadAnimations(unsigned int blockSize) const override;

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
