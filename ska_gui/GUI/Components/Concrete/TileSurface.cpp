#include "TileSurface.h"

void ska::TileSurface::renderTileSurfaceCache(const Rectangle& box, ska::Renderer& renderer) const {
	const auto isSmallX = box.w / ska::TileSurface::TILE_SIZE < 2;
	const auto isSmallY = box.h / ska::TileSurface::TILE_SIZE < 2;

	auto step = ska::Point<float>{ 
		isSmallX ? (box.w / 2.f) : static_cast<float>(TileSurface::TILE_SIZE), 
		isSmallY ? (box.h / 2.f) : static_cast<float>(TileSurface::TILE_SIZE) 
	};
	
	if (step.x < 1) {
		step.x = 1;
	}
	
	if (step.y < 1) {
		step.y = 1;
	}
	
	for (auto i = 0; i < box.w; i += step.x) {
		for (auto j = 0; j < box.h; j += step.y) {
			const auto atLeastSecondClippingX = (i / step.x >= 1);
			const auto lastClippingX = ((i + step.x >= box.w) && (i / (box.w - step.x) >= 1));

			const auto atLeastSecondClippingY = (j / step.y >= 1);
			const auto lastClippingY = ((j + step.y >= box.h) && (j / (box.h - step.y) >= 1));

			auto backgroundTileClip = Rectangle{ 0, 0, static_cast<int>(step.x), static_cast<int>(step.y) };
			backgroundTileClip.x = atLeastSecondClippingX * (2 * TileSurface::TILE_SIZE - step.x) + lastClippingX * TileSurface::TILE_SIZE;
			backgroundTileClip.y = atLeastSecondClippingY * (2 * TileSurface::TILE_SIZE - step.y) + lastClippingY * TileSurface::TILE_SIZE;

			const auto renderPos = ska::Point<int>{
				lastClippingX ? (i - static_cast<int>((i + step.x) - box.w)) : i,
				lastClippingY ? (j - static_cast<int>((j + step.y) - box.h)) : j
			};
			renderer.render(m_menuTiles, renderPos.x, renderPos.y, &backgroundTileClip);
		}
	}
	
}

const ska::Texture& ska::TileSurfaceCache::getTexture(Renderer& renderer) const {
	if (!m_hasToBeRefreshed) {
		return m_cache;
	}

	const auto& box = m_parent.getBox();

	m_parent.m_menuTiles.setBlendMode(SDL_BLENDMODE_NONE);
	m_parent.m_menuTiles.setAlpha(255);

	std::lock_guard<std::mutex> guard{ m_mutex };
	m_cache.loadAsTarget(renderer, box.w, box.h);
	m_cache.setBlendMode(SDL_BLENDMODE_BLEND);
	m_cache.setAlpha(m_parent.m_alpha);

	renderer.setRenderTarget(&m_cache);
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);

	const auto level = box.w / TileSurface::TILE_SIZE;
	m_parent.renderTileSurfaceCache(box, renderer);
	renderer.setRenderTarget(nullptr);
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);

	m_hasToBeRefreshed = false;
	return m_cache;
	
}

ska::TileSurface::TileSurface(Widget& parent, const Rectangle& box, const std::string& styleName) :
	Widget(parent, { box.x, box.y }),
	m_menuTiles(styleName),
	m_cache(*this) {
	TileSurface::setWidth(box.w);
	TileSurface::setHeight(box.h);
}

void ska::TileSurface::setWidth(unsigned int width) {
	Widget::setWidth(width);
	m_cache.invalidate();
}

void ska::TileSurface::setHeight(unsigned int height) {
	Widget::setHeight(height);
	m_cache.invalidate();
}

void ska::TileSurface::setOpacity(const uint8_t opacity) {
	m_alpha = opacity;
	m_cache.invalidate();
}

void ska::TileSurface::render(Renderer& renderer) const {
	const auto& pos = this->getAbsolutePosition();
	const Texture& t = m_cache.getTexture(renderer);
	renderer.render(t, pos.x, pos.y, nullptr);
}

