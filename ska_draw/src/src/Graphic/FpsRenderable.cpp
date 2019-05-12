#include "FpsRenderable.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/Draw/Renderer.h"

ska::FpsRenderable::FpsRenderable() :
	m_t0(0), m_toggleVisible(true) {

}

void ska::FpsRenderable::update(const double value) {
	if (TimeUtils::getTicks() - m_t0 > DELAY) {
		if(value < 0) {
			m_fps.loadFromText(34, "inf.", Color(255, 0, 0, 0));
		} else {
			m_fps.loadFromText(34, StringUtils::intToStr(static_cast<int>(value)), Color(255, 0, 0, 0));
		}
		
		m_t0 = TimeUtils::getTicks();
	}
}

bool ska::FpsRenderable::isVisible() const {
	return m_toggleVisible;
}

void ska::FpsRenderable::setVisible(bool v) {
	m_toggleVisible = v;
}

void ska::FpsRenderable::render(ska::Renderer& renderer) const {
	renderer.render(m_fps, 0, 0, nullptr, 0.);
}
