#pragma once
#include "Graphic/Texture.h"
#include "../World/TileRenderable.h"
#include "Utils/Vector2.h"

namespace ska {
	class TilesetLoader;

	class TilesetRenderable : public MovableNonCopyable {
	public:
		explicit TilesetRenderable(unsigned int tileSize, const TilesetLoader& loader);
		TilesetRenderable(TilesetRenderable&&) = default;

		void render(const Renderer& renderer, Point<int> pos, const TileRenderable& block) const;
		const TileRenderable& getTile(Point<int> posCorr) const;
		const Texture& getTexture() const;

		~TilesetRenderable() override = default;

	private:
		void load(const TilesetLoader& loader);

		Vector2<TileRenderable> m_blocks{};
		Texture m_tileset;
		const std::string m_tilesetName;
		const unsigned int m_tileSize;
	};
}
