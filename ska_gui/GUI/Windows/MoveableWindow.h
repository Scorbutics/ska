#pragma once
#include "DynamicWindowIG.h"
#include "../Components/Concrete/Button.h"
#include "../Components/Concrete/ButtonQuit.h"
#include "../Events/HoverEvent.h"
#include "../GUI.h"

namespace ska {

	template <class ...HL>
	class MoveableWindow : public DynamicWindowIG<HL...> {
	public:
		MoveableWindow(MouseObservable& guiObservable, KeyObservable& keyboardObservable, const Rectangle& box, const std::string& styleName = "") :
			DynamicWindowIG<HL...>(&guiObservable, &keyboardObservable, box, styleName),
			m_moving(false) {
			initHandlers();
		}

		MoveableWindow(Widget& parent, const Rectangle& box, const std::string& styleName = "") :
			DynamicWindowIG<HL...>(parent, box, styleName),
			m_moving(false) {
			initHandlers();
		}

		void setWidth(unsigned int w) override {
			DynamicWindowIG<HL...>::setWidth(w);
			m_dragBar->setWidth(w - ska::WindowIG<>::TAILLEBLOCFENETRE / 2);
			m_dragBar->setClip(Rectangle{ 0, 0, static_cast<int>(w), ska::WindowIG<>::TAILLEBLOCFENETRE / 2 });
			m_quit->move(ska::Point<int>{static_cast<int>(w - ska::WindowIG<>::TAILLEBLOCFENETRE / 2), m_quit->getBox().y});
		}

	private:
		void initHandlers() {
			const auto& clip = Rectangle{ 0, 0, this->getBox().w, ska::WindowIG<>::TAILLEBLOCFENETRE / 2 };
			m_dragBar = &this->template addWidget<Button>(Point<int>(), GUI::MENU_DEFAULT_THEME_PATH + "button", &clip, [&](Widget* tthis, ClickEvent& e) {
				if (e.getState() == MOUSE_CLICK) {
					m_moving = true;
					m_offsetWindowOrigin = e.getPosition(*tthis);
				} else if (e.getState() == MOUSE_RELEASE) {
					m_moving = false;
				}
			});
			auto wButtonWidth = this->getBox().w - ska::WindowIG<>::TAILLEBLOCFENETRE / 2;
			m_dragBar->setWidth((wButtonWidth > 0) ? wButtonWidth : this->getBox().w);
			m_dragBar->setHeight(ska::WindowIG<>::TAILLEBLOCFENETRE / 2);
			m_dragBar->template addHandler<HoverEventListener> ([&](Widget*, HoverEvent& e) {
				if (m_moving && e.getState() != MOUSE_ENTER) {
					const auto& clickAbsPos = e.getMousePosition();
					const auto& newPos = clickAbsPos - m_offsetWindowOrigin;
					this->move(newPos);
				}
			});

			m_quit = &this->template addWidget<ButtonQuit>(Point<int>(this->getBox().w - ska::WindowIG<>::TAILLEBLOCFENETRE / 2, 0), GUI::MENU_DEFAULT_THEME_PATH + "close_button");
			m_quit->setPriority(std::numeric_limits<int>::max() - 1);

			m_dragBar->setPriority(std::numeric_limits<int>::max());

		}

		bool m_moving;
		Point<int> m_offsetWindowOrigin;
		Button* m_dragBar = nullptr;
		Button* m_quit = nullptr;
	};
}
