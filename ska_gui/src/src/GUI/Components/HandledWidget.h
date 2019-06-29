#pragma once
#include <bitset>
#include <tuple>
#include <array>

#include "Widget.h"
#include "../Events/IWidgetEvent.h"
#include "../Utils/WidgetMaskHelper.h"
#include "Base/Meta/TupleUtils.h"
#include "Logging/Logger.h"

static constexpr const auto SKA_GUI_MAX_WIDGET_EVENT_LISTENER = 16u;

namespace ska {

	using ListenerMask = std::bitset<SKA_GUI_MAX_WIDGET_EVENT_LISTENER>;
	using MaskUnit = unsigned int;

	//TODO do we really need this below ?
	class HandledWidgetMask {
	public:
		HandledWidgetMask() = default;
		virtual ~HandledWidgetMask() = default;

		virtual	bool isMaskEmpty() const = 0;
	};

	//TODO move to another file
	class HandlerNotifier {
	public:
		HandlerNotifier(Widget& target, IWidgetEvent& e) : m_event(e), m_target(target), m_result(false) {}

		void operator=(const HandlerNotifier&) = delete;

		template<class T>
		void operator() (T&& t) {
			m_result |= t.notifyGeneric(m_target, m_event);
		}

		bool hasBeenNotified() const {
			return m_result;
		}

	private:
		IWidgetEvent& m_event;
		Widget& m_target;
		bool m_result;
	};


	//TODO a "mask event listener" & "event listener" factory
	template <class W, class ...HL>
	class HandledWidgetTrait : 
		public HandledWidgetMask {
	private:
		static constexpr auto DefaultMaskIndex = sizeof ...(HL) - 1;
	public:

		template<class L, class EH>
		void addHandler(const EH& eh) {
			meta::get<L>(m_handlers).addHandler(eh);
		}

		template<class L, class EH>
		void addHeadHandler(const EH& eh) {
			meta::get<L>(m_handlers).addHeadHandler(eh);
		}

		bool isMaskEmpty() const override {
			return m_mask.none();
		}

		~HandledWidgetTrait() override = default;
		
	protected:
		HandledWidgetTrait() :
			m_target(*static_cast<W*>(this)),
			m_currentMaskIndex(DefaultMaskIndex),
			m_handlers(std::make_tuple<HL...>(instantiateHandler<HL>()...)) {
			static_assert(std::is_base_of<Widget, W>::value, "Must be derived from Widget");
			/* Bracket initializer trick */
			int _[] = { 0, (buildMask<HL>(), 0)... };
			(void)_;
		}

		bool tryTriggerHandlers(IWidgetEvent& e) {
			if (!accept(e)) {
				return false;
			}

			auto maskIndex = static_cast<std::size_t>(m_maskHandlerIndexes[e.getMask()]);
			auto hn = HandlerNotifier{ m_target, e };
			meta::visit_element_at_index(m_handlers, maskIndex, hn);
			return hn.hasBeenNotified();
		}

		bool accept(IWidgetEvent& e) {
			if (!m_target.isVisible()) {
				return false;
			}

			return m_mask[e.getMask()];
		}

	private:
		int m_currentMaskIndex {};
		std::array<std::size_t, SKA_GUI_MAX_WIDGET_EVENT_LISTENER> m_maskHandlerIndexes {};
		ListenerMask m_mask;
		std::tuple<HL...> m_handlers;
		W& m_target;

		template<class T>
		T instantiateHandler() {
			const MaskUnit& mu = WidgetMaskEvent<T>::MASK_ID();
			m_maskHandlerIndexes[mu] = m_currentMaskIndex;
			m_currentMaskIndex--;
			return T();
		}

		template <class T>
		void buildMask() {
			const unsigned int mask = WidgetMaskEvent<T>::MASK_ID();
			if (mask >= m_mask.size()) {
				throw std::runtime_error("Too many listeners are used. Unable to continue.");
			}
			m_mask[mask] = true;
		}
	};

	//Default no event listener implementation
	template <class W>
	class HandledWidgetTrait<W> :
		public HandledWidgetMask {
		
	protected:
		HandledWidgetTrait() : m_target(*static_cast<W*>(this)) {
			static_assert(std::is_base_of<Widget, W>::value, "Must be derived from Widget");
		}
		bool tryTriggerHandlers(IWidgetEvent&) { return false; }
		virtual	bool isMaskEmpty() const override { return true; }
		bool accept(IWidgetEvent&) const{ return !m_target.isVisible(); }
		~HandledWidgetTrait() override = default;
	private:
		Widget& m_target;
	};
}
