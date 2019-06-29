#pragma once
#include <mutex>
#include "../Widget.h"
#include "Graphic/Texture.h"
#include "Core/Draw/Renderer.h"

namespace ska {
	class TileSurface;

	struct TileSurfaceCache {
		TileSurfaceCache(TileSurface& parent) : 
			m_parent(parent) {}

		void invalidate() {
			m_hasToBeRefreshed = true;
		}

		const Texture& getTexture(Renderer& renderer) const;
	
	private:
		TileSurface& m_parent;
		mutable std::mutex m_mutex;
		mutable Texture m_cache;
		mutable bool m_hasToBeRefreshed = true;
	};

	class TileSurface :
		public Widget {
		static constexpr auto TILE_SIZE = 32;
		friend struct TileSurfaceCache;
	public:
		TileSurface(WidgetPanel& parent, const Rectangle& box, const std::string& styleName);
		virtual ~TileSurface() = default;
		
		void setWidth(unsigned int width) override;
		void setHeight(unsigned int height) override;
		void setOpacity(uint8_t opacity);
		void render(Renderer& renderer) const override;
		bool notify(IWidgetEvent& e) override { return false; }
	private:
		void renderTileSurfaceCache(const Rectangle& box, Renderer& renderer) const;
		Texture m_menuTiles;
		TileSurfaceCache m_cache;
		uint8_t m_alpha = 255;
	};
}