#pragma once
#include <algorithm>
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

		// Removes (and destroys) a widget of this panel.
		// The widget is looked for among the handled (interactive) widgets and the other ones, whatever the static type
		// used to remove it (ex : a window removed as a WidgetPanel, although it was added as an interactive window).
		template <class SubWidget>
		bool removeWidget(SubWidget* w) {
			Widget* widget = w;
			// The raw pointers are removed first : the widget is destroyed with its owner
			auto removed = ska::VectorUtils::removeValue<std::vector<Widget*>, Widget*>(m_addedSortedWidgets, widget);
			removed |= ska::VectorUtils::removeValue<WidgetContainer<Widget*>, Widget*>(m_globalList, widget);
			removed |= RemoveOwned(m_handledWidgets, widget);
			removed |= RemoveOwned(m_widgets, widget);
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

		template <class Container>
		static bool RemoveOwned(Container& container, Widget* widget) {
			auto owner = std::find_if(container.begin(), container.end(), [widget](const std::unique_ptr<Widget>& w) {
				return w.get() == widget;
			});
			if (owner == container.end()) {
				return false;
			}
			container.erase(owner);
			return true;
		}
		
		void organizeHandledWidgets();
		void sortZIndexWidgets();

		std::vector<std::unique_ptr<Widget>> m_widgets;
		WidgetContainer<std::unique_ptr<Widget>> m_handledWidgets;
		std::vector<Widget*> m_addedSortedWidgets;
		WidgetContainer<Widget*> m_globalList;
	};

}
