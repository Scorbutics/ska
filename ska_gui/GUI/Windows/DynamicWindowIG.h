#pragma once
#include "../Components/Hoverable.h"
#include "../Utils/MouseObserver.h"
#include "../Utils/KeyObserver.h"
#include "../Utils/KeyObservable.h"
#include "../Utils/MouseObservable.h"
#include "../Events/ClickEventListener.h"
#include "../Components/WidgetPanel.h"

namespace ska {

	class KeyEvent;
	class HoverEvent;
	class ClickEvent;

	template <class ...HL>
	class DynamicWindowIG :
		public WidgetPanel<ClickEventListener, HoverEventListener, HL...>,
		public HoverStateController<DynamicWindowIG<HL...>>,
		public MouseObserver,
		public KeyObserver {
		using Parent = WidgetPanel<ClickEventListener, HoverEventListener, HL...>;
	public:
		DynamicWindowIG(Widget& parent, const Rectangle& box) :
			Parent(parent),
			MouseObserver(std::bind(&DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)) {
				this->move(box);
				this->setWidth(box.w);
				this->setHeight(box.h);
		}

		DynamicWindowIG(MouseObservable* guiObservable, KeyObservable* keyObservable, const Rectangle& box) :
			MouseObserver(std::bind(&DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)),
			m_guiObservable(guiObservable),
			m_keyObservable(keyObservable) {

			m_guiObservable->HoverObservable::addObserver(*this);
			m_guiObservable->ClickObservable::addObserver(*this);
			m_keyObservable->addObserver(*this);
		}

		DynamicWindowIG<HL...>& operator=(const DynamicWindowIG<HL...>&) = delete;

		virtual ~DynamicWindowIG() {
			if (m_guiObservable != nullptr) {
				m_guiObservable->HoverObservable::removeObserver(*this);
				m_guiObservable->ClickObservable::removeObserver(*this);
			}

			if (m_keyObservable != nullptr) {
				m_keyObservable->removeObserver(*this);
			}
		}

		void setWidth(unsigned int w) override {
			Parent::setWidth(w);
			if (m_background != nullptr) {
				m_background->setWidth(w);
			}
		}

		void setHeight(unsigned int h) override {
			Parent::setHeight(h);
			if (m_background != nullptr) {
				const auto& requiredHeight = static_cast<int>(h) - m_offsetTop;
				m_background->setHeight(requiredHeight <= 0 ? 1 : requiredHeight);
			}
		}

		template <class Widget, class ... Args>
		Widget& setBackground(Args&& ... args) {
			if (m_background != nullptr) {
				this->removeWidget(m_background);
			}
			auto& typedWidget = this->template addWidget<Widget>(std::forward<Args>(args)...);
			m_background = &typedWidget;
			m_background->move({ 0, m_offsetTop });
			m_background->setWidth(this->getBox().w);
			const auto& requiredHeight = this->getBox().h - m_offsetTop;
			m_background->setHeight(requiredHeight <= 0 ? 1 : requiredHeight);
			return typedWidget;
		}

	protected:
		bool keyEvent(KeyEvent& e) {
			return this->notify(e);
		}

		bool hoverEvent(HoverEvent& e) {
			return this->notify(e);
		}

		bool clickEvent(ClickEvent& e) {
			return this->notify(e);
		}

		void setOffsetTop(const int offsetTop) {
			m_offsetTop = offsetTop;
		}

	private:
		MouseObservable *const m_guiObservable = nullptr;
		KeyObservable *const m_keyObservable = nullptr;
		int m_offsetTop = 0;
		Widget* m_background = nullptr;
	};
}
