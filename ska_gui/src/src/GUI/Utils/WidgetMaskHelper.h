#pragma once
#include <cstddef>

namespace ska {
	class WStaticCounterGlobal {
	public:
		static unsigned int getCounterAndInc();
	};

	template <class Widget>
	class WidgetMaskEvent {
	public:
		static std::size_t MASK_ID();
	};

}

#define SKA_DECLARE_GUI_EVENT(GuiEvent) template <> std::size_t ska::WidgetMaskEvent<GuiEvent>::MASK_ID();
#define SKA_DECLARE_TEMPLATED_GUI_EVENT(GuiEvent) template <class T>\
class ska::WidgetMaskEvent<GuiEvent<T>> {\
public:\
	static std::size_t MASK_ID();\
};

#define SKA_DEFINE_GUI_EVENT(GuiEvent) template <> std::size_t ska::WidgetMaskEvent<GuiEvent>::MASK_ID() { \
static std::size_t id = WStaticCounterGlobal::getCounterAndInc(); \
return id; \
}
