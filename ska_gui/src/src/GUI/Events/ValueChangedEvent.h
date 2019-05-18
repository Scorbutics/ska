#pragma once

#include "ValueChangedEventListener.h"
#include "WidgetEvent.h"
#include "GUI/Utils/WidgetMaskHelper.h"

SKA_DECLARE_TEMPLATED_GUI_EVENT(ska::ValueChangedEventListener);

namespace ska {
	
	template <class T>
	class ValueChangedEvent : public WidgetEvent<ValueChangedEvent<T>, ValueChangedEventListener<T>> {
	public:
		ValueChangedEvent(T oldValue, T value) :
			m_oldValue(oldValue), m_value(value) {
		}

		void operator=(const ValueChangedEvent&) = delete;

		const T& getValue() const {
			return m_value;
		}
		
		const T& getOldValue() const {
			return m_oldValue;
		}
		
		virtual unsigned int getMask() const override {
			return WidgetMaskEvent<ValueChangedEventListener<T>>::MASK_ID();
		}

		virtual bool affects(const Widget&) const override {
			if (ValueChangedEvent<T>::stopped() != NOT_STOPPED) {
				return false;
			}
			return true;
		}

		virtual ~ValueChangedEvent() = default;

	private:
		const T m_oldValue;
		const T m_value;
	};

}

