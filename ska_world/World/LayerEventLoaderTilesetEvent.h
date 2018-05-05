#pragma once
#include "LayerEventLoader.h"
#include "TilesetEvent.h"

namespace ska {
	class CollisionProfile;
	class Layer;

	class LayerEventLoaderTilesetEvent :
		public LayerEventLoader {

	public:
		explicit LayerEventLoaderTilesetEvent(const CollisionProfile& collisionProfile, std::size_t layer, const TilesetEvent& tilesetEvent);
		Vector2<ScriptPack> loadPositioned(unsigned int width, unsigned int height) const override;
		ScriptPack loadGlobal() const override;
        const std::string& getName() const override;

	private:
		const CollisionProfile& m_collisionProfile;
		const std::size_t m_layer;
		const TilesetEvent& m_tilesetEvent;
		std::string m_fileName;
		const std::string m_name;
	};
}
