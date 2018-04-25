#pragma once
#include "../Rectangle.h"

namespace ska {
	class Renderer;
	template <class T>
	struct Point;

	class Renderable {
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual void render(const Renderer& renderer, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter = nullptr) const = 0;
	};
}