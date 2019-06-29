#pragma once

#include "Core/Point.h"
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/Utils/RectangleUtils.h"
#include "Base/Values/MovableNonCopyable.h"

namespace ska {
	class IWidgetEvent;
	class WidgetPanel;

	enum class ButtonState {
		NONE,
		PRESSED,
		HOVER,
		ENTER,
		DISABLED
	};
	
	class Widget :
		public DrawableFixedPriority,
		public MovableNonCopyable {

	public:
		Widget() = default;
		explicit Widget(WidgetPanel& parent);
		Widget(WidgetPanel& parent, const Point<int>& position);
		virtual ~Widget() = default;

		bool isFocused() const;
		bool isVisible() const;
		bool isAParent(const Widget& w) const;
		void focus(bool f);
		void show(bool sh);
		WidgetPanel* getParent() const;
		const Rectangle& getBox() const;
		Point<int> getAbsolutePosition() const;
		Point<int> getRelativePosition() const;

		virtual void setWidth(unsigned int w);
		virtual void setHeight(unsigned int h);
		virtual void move(const Point<int>& pos);
		virtual bool notify(IWidgetEvent& e) = 0;
		virtual bool directNotify(IWidgetEvent& e) {
			return notify(e);
		}

		void setPriority(int p) override;

	private:
		WidgetPanel* m_parent = nullptr;;
		Rectangle m_box = { 0, 0, 1, 1 };
		bool m_visible = true;
		bool m_focused = false;
	};

}
