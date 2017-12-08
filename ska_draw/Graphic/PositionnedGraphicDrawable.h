#pragma once
#include "Draw/Drawable.h"
#include "Draw/Renderer.h"

namespace ska {
	class PositionnedGraphicDrawable : public Drawable {

	public:
		PositionnedGraphicDrawable(Renderable& gc, int posx, int posy, int priority, int priority2D) :
			m_graphic(gc),
			m_priority(priority),
			m_priority2D(priority2D)  {
			m_positionx = posx;
			m_positiony = posy;
		}

		void operator=(const PositionnedGraphicDrawable&) = delete;

		virtual void render(const Renderer& renderer) const override {
			m_graphic.render(renderer, m_positionx, m_positiony);
		}

		virtual int getPriority() const override {
			return m_priority;
		}

		virtual int getPriority2D() const override {
			return m_priority2D;
		}

		virtual bool isVisible() const override {
			return true;
		}
		virtual ~PositionnedGraphicDrawable(){}

	private:
		Renderable& m_graphic;
		int m_positionx;
		int m_positiony;
		int m_priority;
		int m_priority2D;
	};
}
