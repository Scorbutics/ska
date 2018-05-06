#pragma once
#include <memory>

namespace ska {
	struct Rectangle;
	class DrawableContainer;

	class LayerRenderable {
	public:
		virtual void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) = 0;
		virtual ~LayerRenderable() = default;
	};
	using LayerRenderablePtr = std::unique_ptr<LayerRenderable>;
}