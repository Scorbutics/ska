#pragma once
#include <memory>
#include "TilesetRenderable.h"
#include "TilesetCorrespondanceMapper.h"
#include "TilesetLoader.h"
#include "TilesetEvent.h"

namespace ska {
	struct Tile;
	class Tileset : public MovableNonCopyable {
	public:
		Tileset(unsigned int tileSize, const TilesetLoader& loader, const TilesetEventLoader& loaderEvent);
		Tileset(Tileset&&) = default;

		Tile& getTile(const Point<int>& coordinates);
		const Tile& getTile(const Point<int>& coordinates) const;

		TileAnimation& getAnimation(const Point<int>& coordinates);
		const TileAnimation& getAnimation(const Point<int>& coordinates) const;

		const Texture& getTexture() const;
		const std::string& getName() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;

		void update();

		~Tileset() override = default;
		unsigned int getTileSize() const;
		const TilesetEvent& getTilesetEvent() const;

		void setPropertiesBitMask(std::size_t x, std::size_t y, unsigned int bitmask);

	private:
		std::string m_tilesetName;
		const unsigned int m_tileSize;

		TilesetRenderable m_renderable;
		TilesetEvent m_event;
		Vector2<Tile> m_blocks{};
	};
	using TilesetPtr = std::unique_ptr<Tileset>;
}
