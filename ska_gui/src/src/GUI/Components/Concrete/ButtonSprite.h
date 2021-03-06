#pragma once

#include "Button.h"

namespace ska {
	class ClickEvent;

	class ButtonSprite : 
		public Button {

	public:
		ButtonSprite(WidgetPanel& parent, Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback);
		ButtonSprite(WidgetPanel& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& imagePath, const Rectangle& clip, ClickEventHandler const& callback);
		virtual ~ButtonSprite() = default;

		void replaceWith(const std::string& path, const Rectangle& clip);

		void render(Renderer& renderer) const override;
		virtual void move(const Point<int>& pos) override;

	private:
		Texture m_img;
		Rectangle m_rect;
		Rectangle m_clip;
	};
}
