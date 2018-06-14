#pragma once
#include "../Widget.h"
#include "Graphic/Texture.h"

namespace ska {
	class TileSurface :
		public Widget {
		static constexpr auto TAILLEBLOCFENETRE = 32;
	public:
		TileSurface(Widget& parent, const std::string& styleName);
		virtual ~TileSurface() = default;

		void render(const Renderer& renderer) const override;
	
	private:
		Texture m_menuTiles;
	};
}