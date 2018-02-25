#pragma once
#include <vector>
#include "Graphic/SDLRenderer.h"
#include "Draw/DrawableFixedPriority.h"

template <class T>
class Polygon : public ska::DrawableFixedPriority {
public:
	Polygon() = default;

	template<class Container>
	Polygon(const Container& c, const unsigned int zoom = 1) {
		m_points.reserve(c.size());
		for(const auto& p : c) {
			m_points.push_back(p * zoom);
		}
	}

	~Polygon() override = default;

	void render(const ska::Renderer& renderer) const override {
		auto lastPointIt = m_points.cbegin();
		
		auto pIt = m_points.cbegin();
		for (++pIt; pIt != m_points.cend(); ++pIt) {
			renderer.drawColorLine(renderer.getDefaultColor(), *lastPointIt, *pIt);
			lastPointIt = pIt;
		}
	}

	bool isVisible() const override {
		return m_visible;
	}

private:
	std::vector<ska::Point<T>> m_points;
	bool m_visible = true;
};

