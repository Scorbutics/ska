#include "Utils/TimeUtils.h"
#include "Animation.h"

ska::Animation::Animation() {
	m_offset = Rectangle{0, 0, 0, 0};
	m_offsetBase = m_offset;
	m_frameNumber = 1;
	m_count = TimeUtils::getTicks();
	m_countMAX = static_cast<unsigned int>(-1);
	m_position = 0;
	m_isVertical = false;
	m_sensAni = 1;
	m_active = true;
	m_cycles = 0;
}

ska::Animation::Animation(unsigned int delay, int frameNumber, bool isVertical, int x, int y, unsigned int width, unsigned int height) {
	Rectangle r;
	r.x = x;
	r.y = y;
	r.w = width;
	r.h = height;

	m_offset = r;
	m_offsetBase = m_offset;
    m_frameNumber = frameNumber;
	if (m_frameNumber == 0) {
		m_frameNumber = 1;
	}
    m_count = TimeUtils::getTicks();
    m_countMAX = delay;
    m_position = 0;
    m_isVertical = isVertical;
    m_sensAni = 1;
    m_active = true;
	m_cycles = 0;
}

void ska::Animation::setVertical(const bool b) {
	m_isVertical = b;
}

void ska::Animation::stop(bool x) {
    m_active = !x;
}

void ska::Animation::setDelay(unsigned int x) {
    m_countMAX = x;
}

void ska::Animation::setOffsetAndFrameSize(Rectangle s) {
    m_offsetBase = s;
    m_offset = s;

	if (m_isVertical) {
		m_offset.y = m_offsetBase.y + m_position * m_offsetBase.h;
	} else {
		m_offset.x = m_offsetBase.x + m_position * m_offsetBase.w;
	}
}

ska::Rectangle ska::Animation::getOffsetBase() const {
	return m_offsetBase;
}

ska::Rectangle ska::Animation::getCurrentFrame() const {
    return m_offset;
}

void ska::Animation::setFrames(const unsigned int framesNumber) {
	m_frameNumber = framesNumber;
}

void ska::Animation::switchToFrame(unsigned int number) {
	m_position = number;

    if(m_position >= m_frameNumber) {
        m_sensAni = -1;
        m_position = m_frameNumber-2;
    }

	if(m_position <= 0) {
        m_sensAni = 1;
        m_position = 0;
    }

	if (m_isVertical) {
		m_offset.y = m_offsetBase.y + m_position * m_offsetBase.h;
	} else {
		m_offset.x = m_offsetBase.x + m_position * m_offsetBase.w;
	}

}

void ska::Animation::nextFrame() {
    m_position += m_sensAni;
	switchToFrame(m_position);
}

ska::Rectangle ska::Animation::updateFrame() {
	if (!m_active) {
		return m_offset;
	}

    if(TimeUtils::getTicks() - m_count > m_countMAX) {
        m_count = TimeUtils::getTicks();
		nextFrame();
    }

    return m_offset;
}

