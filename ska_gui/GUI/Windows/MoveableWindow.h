#pragma once
#include "DynamicWindowIG.h"
#include "../Components/Concrete/Button.h"
#include "../Components/Concrete/Label.h"
#include "../Components/Concrete/ButtonQuit.h"
#include "../Events/HoverEvent.h"
#include "../GUI.h"

namespace ska {
	
	template <class ...HL>
	class MoveableWindow : public DynamicWindowIG<HL...> {
		static constexpr auto BAR_HEIGHT = 16;
	public:
		MoveableWindow(MouseObservable& guiObservable, KeyObservable& keyboardObservable, const Rectangle& box, const std::string& title = "") :
			DynamicWindowIG<HL...>(&guiObservable, &keyboardObservable, box),
			m_moving(false) {
			initHandlers(title);
			setOffsetTop(BAR_HEIGHT);
		}

		MoveableWindow(Widget& parent, const Rectangle& box, const std::string& title = "") :
			DynamicWindowIG<HL...>(parent, box),
			m_moving(false) {
			initHandlers(title);
			setOffsetTop(BAR_HEIGHT);
		}

		void setWidth(unsigned int w) override {
			m_dragBar->setWidth(w - BAR_HEIGHT);
			m_dragBar->setClip(Rectangle{ 0, 0, static_cast<int>(w), BAR_HEIGHT });
			m_quit->move(ska::Point<int>{static_cast<int>(w - BAR_HEIGHT), m_quit->getBox().y});
			m_title->move({ this->getBox().w / 2 - m_title->getBox().w / 2, m_title->getBox().y });
			DynamicWindowIG<HL...>::setWidth(w);
		}

		void setTitle(std::string title) {
			m_title->modifyText(title);
		}

	private:
		void initHandlers(const std::string& title) {
			const auto& clip = Rectangle{ 0, 0, this->getBox().w, BAR_HEIGHT };
			m_dragBar = &this->template addWidget<Button>(Point<int>(), GUI::MENU_DEFAULT_THEME_PATH + "button", &clip, [&](Widget* tthis, ClickEvent& e) {
				if (e.getState() == MOUSE_CLICK) {
					m_moving = true;
					m_offsetWindowOrigin = e.getPosition(*tthis);
				} else if (e.getState() == MOUSE_RELEASE) {
					m_moving = false;
				}
			});
			auto wButtonWidth = this->getBox().w - BAR_HEIGHT;
			m_dragBar->setWidth((wButtonWidth > 0) ? wButtonWidth : this->getBox().w);
			m_dragBar->setHeight(BAR_HEIGHT);
			m_dragBar->template addHandler<HoverEventListener> ([&](Widget*, HoverEvent& e) {
				if (m_moving && e.getState() != MOUSE_ENTER) {
					const auto& clickAbsPos = e.getMousePosition();
					const auto& newPos = clickAbsPos - m_offsetWindowOrigin;
					this->move(newPos);
				}
			});

			m_quit = &this->template addWidget<ButtonQuit>(Point<int>(this->getBox().w - BAR_HEIGHT, 0), GUI::MENU_DEFAULT_THEME_PATH + "close_button");
			m_quit->setPriority(std::numeric_limits<int>::max() - 1);

			m_dragBar->setPriority(std::numeric_limits<int>::max());
			m_title = &this->template addWidget<Label>(title.empty() ? " " : title, 11, ska::Point<int>{ 0, 0 });
			m_title->move({ this->getBox().w / 2 - m_title->getBox().w / 2, m_title->getBox().y });
			m_title->setFontColor(255, 255, 255, 255);
		}

		bool m_moving;
		Point<int> m_offsetWindowOrigin;
		Button* m_dragBar = nullptr;
		Button* m_quit = nullptr;
		Label* m_title = nullptr;
	};
}
