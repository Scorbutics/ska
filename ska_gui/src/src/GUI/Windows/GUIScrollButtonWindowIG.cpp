#include "GUI/__internalConfig/LoggerConfig.h"
#include "GUIScrollButtonWindowIG.h"
#include "../GUI.h"
#include "../Components/Concrete/TileSurface.h"

ska::GUIScrollButtonWindowIG::GUIScrollButtonWindowIG(WidgetPanel& parent, const Rectangle& box) :
	TimeScrollableWindowIG<>(parent, box),
	m_realPos(0, 0) {

	auto& tileSurface = addWidget<ska::TileSurface>(Rectangle{ 0, 0, box.w, box.h }, ska::GUI::MENU_DEFAULT_THEME_PATH + "menu.png");
	//tileSurface.setOpacity(OPACITY);
}

ska::WorkNode<ska::TimeScrollableWindowIG<>>& ska::GUIScrollButtonWindowIG::scrollTo(const Point<int>& targetPos, unsigned int speed) {
	if (!isMoving()) {
		m_realPos = getRelativePosition();
	}
	scrollStop();
	return TimeScrollableWindowIG::scrollTo(targetPos, speed);
}

ska::WorkNode<ska::TimeScrollableWindowIG<>>& ska::GUIScrollButtonWindowIG::scrollRewind() {
	scrollStop();
	return TimeScrollableWindowIG::scrollTo(m_realPos, 5);
}

ska::GUIScrollButtonWindowIG::~GUIScrollButtonWindowIG() {

}


