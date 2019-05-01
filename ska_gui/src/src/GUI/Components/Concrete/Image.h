#pragma once
#include <optional>
#include "../Widget.h"
#include "Graphic/Texture.h"


namespace ska {
	class Image : public Widget {
	public:
		Image(Widget& parent, const std::string& imagePath, Point<int> relativePos, bool alpha, Rectangle* clip);
		virtual ~Image() = default;
		void replaceWith(Texture img, Rectangle* clip);
		virtual void setWidth(unsigned int w) override;
		virtual void setHeight(unsigned int h) override;
		void render(Renderer& renderer) const override;

	private:
		Texture m_img;
		std::optional<Rectangle> m_clip;
	};
}
