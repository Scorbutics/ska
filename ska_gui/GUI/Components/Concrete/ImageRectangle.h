#pragma once
#include "../Widget.h"
#include "Graphic/Texture.h"

namespace ska {
	class ImageRectangle : public Widget {
	public:
		ImageRectangle(Widget& parent, Rectangle box, Color c, const Color* outline = nullptr);
		virtual ~ImageRectangle() = default;
		virtual void setWidth(unsigned int w) override;
		virtual void setHeight(unsigned int h) override;
		void render(const Renderer& renderer) const override;

	private:
		Texture m_img;
		Rectangle m_box;
	};
}
