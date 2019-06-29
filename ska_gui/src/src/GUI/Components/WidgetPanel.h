#pragma once
#include <memory>
#include <vector>

#include "../Utils/WidgetContainer.h"
#include "../Utils/WidgetHandlingTrait.h"
#include "Widget.h"

namespace ska {

	class WidgetPanel :
		public Widget {

	public:
		using Widget::Widget;
		WidgetPanel(WidgetPanel& parent) : Widget(parent) {}
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

		void render(Renderer& renderer) const override;
		Widget* backAddedWidget();
		Widget* backWidget();
		void clear();

		void resort();

	protected:
		Widget* getWidget(std::size_t index);
		bool notifyChildren(IWidgetEvent& e);

	private:
		static constexpr auto GuiDefaultDisplayPriority = 65535;
		
		void organizeHandledWidgets();
		void sortZIndexWidgets();

		std::vector<std::unique_ptr<Widget>> m_widgets;
		WidgetContainer<std::unique_ptr<Widget>> m_handledWidgets;
		std::vector<Widget*> m_addedSortedWidgets;
		WidgetContainer<Widget*> m_globalList;
	};

}
