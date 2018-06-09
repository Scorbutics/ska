#pragma once
#include "../Widget.h"
#include "Graphic/Texture.h"
#include "Draw/Color.h"

namespace ska {
	class Label : public Widget {
	public:
		Label(Widget& parent, const std::string& text, int fontSize = 11, Point<int> relativePos = Point<int>{});
		virtual ~Label() = default;

		void setFontColor(int r, int g, int b, int a);
		void modifyText(const std::string& text);
		void setClip(Rectangle* r);
		void render(const ska::Renderer& renderer) const override;
		void clear();

	private:
		std::string m_text;
		unsigned int m_fontSize;
		Texture m_stext;
		Color m_color;
		Rectangle* m_clip;
	};
}
