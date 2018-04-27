#pragma once
#include "LayerEventLoader.h"
#include "TilesetEvent.h"

namespace ska {
	class Layer;

	class LayerEventLoaderTilesetEvent :
		public LayerEventLoader {

	public:
		explicit LayerEventLoaderTilesetEvent(const Layer& layer, const TilesetEvent& tilesetEvent);
		Vector2<ScriptPack> loadPositioned(unsigned int width, unsigned int height) const override;
		ScriptPack loadGlobal() const override;
        const std::string& getName() const override;

	private:
		const Layer& m_layer;
		const TilesetEvent& m_tilesetEvent;
		std::string m_fileName;
	};
}
