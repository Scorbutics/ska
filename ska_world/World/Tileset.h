#pragma once
#include <memory>
#include <optional>
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
		const TileRenderable& getTileRenderable(const Point<int>& coordinates);

		const std::string& getName() const;
		const TilesetRenderable& getRenderable() const;

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		~Tileset() override = default;
		unsigned int getTileSize() const;

	private:
		std::string m_tilesetName;
		const unsigned int m_tileSize;

		TilesetRenderable m_renderable;
		TilesetEvent m_event;
		Vector2<Tile> m_blocks{};
	};
}
