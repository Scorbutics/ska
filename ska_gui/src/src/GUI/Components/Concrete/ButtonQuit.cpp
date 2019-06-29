#include "ButtonQuit.h"
#include "../WidgetPanel.h"

ska::ButtonQuit::ButtonQuit(WidgetPanel& parent, Point<int> relativePos, const std::string& placeHolderStyleName) :
Button(parent, relativePos, placeHolderStyleName, nullptr, [&](Widget*, const ClickEvent& ){
	parent.show(false);
}) {
}
