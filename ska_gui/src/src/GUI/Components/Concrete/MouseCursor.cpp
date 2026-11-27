#include <SDL3/SDL.h>
#include "GUI/__internalConfig/LoggerConfig.h"
#include "MouseCursor.h"
#include "Core/Draw/Renderer.h"
#include "Core/SDLLibrary.h"

ska::MouseCursor::MouseCursor(const std::string& styleName) :
	m_basicCursor(styleName + ".png"),
	m_grabCursor(styleName + "_grab.png"),
	m_currentCursor(&m_basicCursor),
	m_visible(false) {
	SDLLibrary::get().showCursor(false);
}

void ska::MouseCursor::move(const Point<int>& mousePos) {
	m_mousePos = mousePos;
}

bool ska::MouseCursor::isVisible() const{
	return m_visible;
}

void ska::MouseCursor::grab(){
	m_currentCursor = &m_grabCursor;
}

void ska::MouseCursor::basic(){
	m_currentCursor = &m_basicCursor;
}

ska::MouseCursor::~MouseCursor(){}

void ska::MouseCursor::render(Renderer& renderer) const{
	// Not drawn while the mouse is out of the window (ex : the pointer of a virtual display, tools/ska-run)
	if (m_currentCursor != nullptr && SDLLibrary::get().hasMouseFocus()) {
		renderer.render(*m_currentCursor, m_mousePos.x, m_mousePos.y, nullptr);
	}
}
