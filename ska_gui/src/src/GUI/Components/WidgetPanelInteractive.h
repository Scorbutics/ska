#pragma once

#include "HandledWidget.h"
#include "../Utils/WidgetHandlingTrait.h"
#include "WidgetPanel.h"

namespace ska {
	template <class ... HL>
	class WidgetPanelInteractive : 
		public HandledWidgetTrait<WidgetPanelInteractive<HL...>, HL...>,
		public WidgetPanel {
		using ParentTrait = HandledWidgetTrait<WidgetPanelInteractive<HL...>, HL...>;
	public:
		WidgetPanelInteractive() = default;

		explicit WidgetPanelInteractive(WidgetPanel& parent) :
			WidgetPanel(parent) {
		}

		WidgetPanelInteractive(WidgetPanel& parent, const Point<int>& position) :
			WidgetPanel(parent, position) {
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {
			/* If the current WidgetPanel doesn't accept the event, neither of his children do. */
			if (!ParentTrait::accept(e)) {
				return false;
			}

			 auto result = WidgetPanel::notifyChildren(e);
			 result |= ParentTrait::tryTriggerHandlers(e);

			 if (result) {
				 /* Handled by Widget */
				 e.stopPropagation(STOP_WIDGET);
			 }
			 return result;
		}

		virtual bool directNotify(IWidgetEvent& e) override {
			return ParentTrait::tryTriggerHandlers(e);
		}

		virtual ~WidgetPanelInteractive() = default;

	};
}