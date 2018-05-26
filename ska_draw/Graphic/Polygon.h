#pragma once
#include <vector>
#include "SDLRenderer.h"
#include "Draw/DrawableFixedPriority.h"

namespace ska {
	template <class T>
	class Polygon : public ska::DrawableFixedPriority {

		struct LinkedPoint {
			LinkedPoint(const ska::Point<T>& value, const bool linked)
				: value(value),
				linked(linked) {
			}


			ska::Point<T> value;
			bool linked;
		};

	public:
		Polygon() = default;

		template<class Container>
		Polygon(const Container& c, const unsigned int zoom = 1) :
			m_color(255, 0, 0, 255) {
			m_points.reserve(c.size());
			for (const auto& p : c) {
				m_points.push_back({ p * zoom, true });
			}
		}

		Polygon(const std::vector<ska::Rectangle>& c) :
			m_color(255, 0, 0, 255) {
			m_points.reserve(c.size());
			for (const auto& p : c) {
				m_points.push_back({ { p.x, p.y }, true });
				m_points.push_back({ { p.x + p.w, p.y }, true });
				m_points.push_back({ { p.x + p.w, p.y + p.h}, true });
				m_points.push_back({ { p.x, p.y + p.h }, true });
				m_points.push_back({ { p.x, p.y }, false });
			}
		}

		~Polygon() override = default;

		void setOffset(ska::Point<int> offset) {
			m_offset = offset;
		}

		void render(const ska::Renderer& renderer) const override {
			auto lastPointIt = m_points.cbegin();

			auto pIt = m_points.cbegin();

			if (pIt != m_points.cend()) {
				++pIt;
				for (; pIt != m_points.cend(); ++pIt) {
					if ((*lastPointIt).linked) {
						renderer.drawColorLine(m_color, m_offset + (*lastPointIt).value, m_offset + (*pIt).value);
					}
					lastPointIt = pIt;
				}
			}
		}

		void setColor(ska::Color color) {
			m_color = std::move(color);
		}
	private:
		std::vector<LinkedPoint> m_points;
		bool m_visible = true;
		ska::Color m_color;
		ska::Point<int> m_offset;
	};

}