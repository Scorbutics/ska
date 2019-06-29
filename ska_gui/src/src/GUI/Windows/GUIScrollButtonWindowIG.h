#pragma once
#include "Core/Rectangle.h"
#include "Core/Point.h"
#include "TimeScrollableWindowIG.h"

namespace ska {

	class GUIScrollButtonWindowIG : public TimeScrollableWindowIG<> {
	public:
		GUIScrollButtonWindowIG(WidgetPanel& parent, const Rectangle& box);
		virtual WorkNode<TimeScrollableWindowIG<>>& scrollTo(const Point<int>& targetPos, unsigned int speed) override;
		WorkNode<TimeScrollableWindowIG<>>& scrollRewind();
		virtual ~GUIScrollButtonWindowIG();
	private:
		Point<int> m_realPos;

	};
}
