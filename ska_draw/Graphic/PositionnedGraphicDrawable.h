#pragma once
#include <optional>
#include "Draw/Drawable.h"
#include "Draw/Renderer.h"

namespace ska {
	class Renderable;

	class PositionnedGraphicDrawable : 
		public Drawable {
	public:
		PositionnedGraphicDrawable(Renderable& gc, int posx, int posy, int priority, int priority2D) :
			m_graphic(gc),
			m_priority(priority),
			m_priority2D(priority2D), 
			m_position(posx, posy),
			m_angle(0) {
		}

		void operator=(const PositionnedGraphicDrawable&) = delete;

		void render(const Renderer& renderer) const override {
			m_graphic.render(renderer, m_position.x, m_position.y, nullptr, m_angle, m_rotationCenter.has_value() ? &m_rotationCenter.value() : nullptr);
		}

		int getPriority() const override {
			return m_priority;
		}

		int getPriority2D() const override {
			return m_priority2D;
		}

		void move(int x, int y) {
			m_position.x = x;
			m_position.y = y;
		}

		void rotate(Point<int> rotationCenter, double angle) {
			m_rotationCenter = std::move(rotationCenter);
			m_angle = angle;
		}

		void rotateOnItself(double angle) {
			m_rotationCenter = std::optional<ska::Point<int>>();
			m_angle = angle;
		}

		virtual ~PositionnedGraphicDrawable() = default;

	private:
		Renderable& m_graphic;
		int m_priority;
		int m_priority2D;
		ska::Point<int> m_position;
		std::optional<Point<int>> m_rotationCenter;
		double m_angle;
	};
}
