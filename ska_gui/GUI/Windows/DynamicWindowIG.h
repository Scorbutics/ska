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

	private:
		MouseObservable *const m_guiObservable = nullptr;
		KeyObservable *const m_keyObservable = nullptr;
	};
}
