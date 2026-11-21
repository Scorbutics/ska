#pragma once
#include <memory>
#include "Core/Utils/VectorUtils.h"
#include "GUI/Components/HandledWidget.h"
#include "WidgetContainer.h"

namespace ska {
	template <class SubWidget, bool = std::is_base_of<HandledWidgetMask, SubWidget>::value>
	struct WidgetHandlingTrait {
		static void manageHandledAdd(std::unique_ptr<SubWidget>&& w, WidgetContainer<std::unique_ptr<Widget>>& handledWidgets, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			auto widget = static_cast<HandledWidgetMask*>(w.get());
			auto activeWidget = !widget->isMaskEmpty();
			if (activeWidget) {
				ska::Widget* rawW = w.get();
				handledWidgets.push_back(std::move(w));
				globalList.push_back(std::move(rawW));
			} else {
				WidgetHandlingTrait<SubWidget, false>::manageHandledAdd(std::move(w), handledWidgets, widgets, globalList);
			}
		}

	};

	template <class SubWidget>
	struct WidgetHandlingTrait<SubWidget, false> {
		static void manageHandledAdd(std::unique_ptr<SubWidget>&& w, WidgetContainer<std::unique_ptr<Widget>>&, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			ska::Widget* rawW = w.get();
			widgets.push_back(std::move(w));
			globalList.push_back(std::move(rawW));
		}

	};
}
