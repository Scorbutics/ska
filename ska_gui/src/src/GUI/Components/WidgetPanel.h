#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include <algorithm>

#include "HandledWidget.h"
#include "../Utils/WidgetHandlingTrait.h"
#include "../Events/StopType.h"
#include "../Events/IWidgetEvent.h"
#include "Core/Utils/VectorUtils.h"

namespace ska {

	class WidgetPanel :
		public Widget {

	public:
		using Widget::Widget;
		WidgetPanel(Widget& parent) : Widget(parent) {}
		~WidgetPanel() override = default;

		template <class SubWidget, class ... Args>
		SubWidget& addWidget(Args&& ... args) {
			auto w = std::make_unique<SubWidget>(*this, std::forward<Args>(args)...);
			w->setPriority(GuiDefaultDisplayPriority - 1 - static_cast<int>(m_globalList.size()));
			auto& result = static_cast<SubWidget&>(*w.get());
			WidgetHandlingTrait<SubWidget>::manageHandledAdd(std::move(w), m_handledWidgets, m_widgets, m_globalList);
			m_addedSortedWidgets.push_back(&result);
			resort();
			return result;
		}

		template <class SubWidget>
		bool removeWidget(SubWidget* w) {
			auto removed = false;
			removed |= WidgetHandlingTrait<SubWidget>::manageHandledRemove(w, m_handledWidgets, m_widgets, m_globalList);
			removed |= ska::VectorUtils::removeValue<std::vector<Widget*>, Widget*>(m_addedSortedWidgets, w);
			return removed;
		}

		void showWidgets(bool b);
		void render(Renderer& renderer) const override;
		Widget* backAddedWidget();
		Widget* backWidget();
		void clear();

	protected:
		Widget* getWidget(std::size_t index);
		bool notifyChildren(IWidgetEvent& e);

	private:
		static constexpr auto GuiDefaultDisplayPriority = 65535;

		void resort();
		void organizeHandledWidgets();
		void sortZIndexWidgets();

		std::vector<std::unique_ptr<Widget>> m_widgets;
		WidgetContainer<std::unique_ptr<Widget>> m_handledWidgets;
		std::vector<Widget*> m_addedSortedWidgets;
		WidgetContainer<Widget*> m_globalList;
	};


	template <class ... HL>
	class WidgetPanelInteractive : 
		public HandledWidgetTrait<WidgetPanelInteractive<HL...>, HL...>,
		public WidgetPanel {
		using ParentTrait = HandledWidgetTrait<WidgetPanelInteractive<HL...>, HL...>;
	public:
		WidgetPanelInteractive() = default;

		explicit WidgetPanelInteractive(Widget& parent) :
			WidgetPanel(parent) {
		}

		WidgetPanelInteractive(Widget& parent, const Point<int>& position) :
			WidgetPanel(parent, position) {
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {
			/* If the current WidgetPanel doesn't accept the event, neither of his children do. */
			if (!ParentTrait::accept(e)) {
				return false;
			}

			 auto result = WidgetPanel::notifyChildren(e);
			 result |= directNotify(e);

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
