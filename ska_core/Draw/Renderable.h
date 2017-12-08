#pragma once
#include "../Rectangle.h"

namespace ska {
	class Renderer;
	class Renderable {
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual int render(const Renderer& renderer, int posX, int posY, const Rectangle* clip = nullptr) const = 0;
	};
}