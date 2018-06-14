#include "TileSurface.h"
#include "Draw/Renderer.h"

ska::TileSurface::TileSurface(Widget& parent, const std::string& styleName) :
	Widget(parent),
	m_menuTiles(styleName) {
}

void ska::TileSurface::render(const Renderer & renderer) const {
	auto backgroundTileClip = Rectangle { 0, 0, TAILLEBLOCFENETRE, TAILLEBLOCFENETRE };
	const auto& box = this->getBox();
	const auto& pos = this->getAbsolutePosition();

	for (auto i = 0; i < box.w; i += backgroundTileClip.w) {
		for (auto j = 0; j < box.h; j += backgroundTileClip.h) {
			const auto backgroundTilePos = Point<int>{ pos.x + i, pos.y + j };
			const auto xcondition1 = (i / backgroundTileClip.w >= 1);
			const auto xcondition2 = ((box.w != backgroundTileClip.w) && (i / (box.w - backgroundTileClip.w) >= 1));

			const auto ycondition1 = (j / backgroundTileClip.h >= 1);
			const auto ycondition2 = ((box.h != backgroundTileClip.h) && (j / (box.h - backgroundTileClip.h) >= 1));

			backgroundTileClip.x = xcondition1 * backgroundTileClip.w + xcondition2 * backgroundTileClip.w;
			backgroundTileClip.y = ycondition1 * backgroundTileClip.h + ycondition2 * backgroundTileClip.h;

			renderer.render(m_menuTiles, backgroundTilePos.x, backgroundTilePos.y, &backgroundTileClip);
		}
	}
}

