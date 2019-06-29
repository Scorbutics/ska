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

	class WidgetPanelSorter :
		public Widget {

	public:
		using Widget::Widget;
		WidgetPanelSorter(Widget& parent) : Widget(parent) {}

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

		void showWidgets(bool b) {
			for (auto& w : m_globalList) {
				w->show(b);
			}
		}

		void render(Renderer& renderer) const override {
			for (auto& w : m_globalList) {
				if (w->isVisible()) {
					w->render(renderer);
				}
				else {
					//First invisible widget found : we stop
					break;
				}
			}
		}

		Widget* backAddedWidget() {
			return m_addedSortedWidgets.empty() ? nullptr : m_addedSortedWidgets.back();
		}

		Widget* backWidget() {
			return m_globalList.empty() ? nullptr : m_globalList.back();
		}

		void clear() {
			m_widgets.clear();
			m_handledWidgets.clear();
			m_globalList.clear();
			m_addedSortedWidgets.clear();
		}

	protected:
		Widget* getWidget(std::size_t index) {
			return m_addedSortedWidgets[index];
		}

		bool notifyChildren(IWidgetEvent& e) {
			organizeHandledWidgets();

			auto result = false;
			auto stopped = false;
			std::size_t cursor = 0;
			for (auto& w : m_handledWidgets) {
				if (!w->isVisible() && !m_handledWidgets.isVisibleAtIndex(cursor)) {
					break;
				}

				const auto nextNotify = w->notify(e);
				result |= nextNotify;
				if (e.stopped() == STOP_WIDGET) {
					stopped = true;
					break;
				}
				cursor++;
			}

			if (stopped) {
				e.stopPropagation(NOT_STOPPED);
			}
			return result || stopped;
		}

	private:
		static constexpr auto GuiDefaultDisplayPriority = 65535;

		void resort() {
			organizeHandledWidgets();
			this->sortZIndexWidgets();
		}

		void organizeHandledWidgets() {
			std::size_t cursor = 0;
			for (auto& w : m_handledWidgets) {
				m_handledWidgets.organize(w, cursor);
				cursor++;
			}
		}

		void sortZIndexWidgets() {
			static const auto comparatorAsc = [](const std::unique_ptr<Widget>& w1, const std::unique_ptr<Widget>& w2) {
				auto v1 = w1->isVisible() ? 0 : 1;
				auto v2 = w2->isVisible() ? 0 : 1;

				if (v1 == v2) {
					return (w1->getPriority() < w2->getPriority());
				}

				return v1 < v2;
			};

			static const auto comparatorDescRaw = [](const Widget* w1, const Widget* w2) {
				auto v1 = w1->isVisible() ? 0 : 1;
				auto v2 = w2->isVisible() ? 0 : 1;

				if (v1 == v2) {
					return (w1->getPriority() > w2->getPriority());
				}

				return v1 < v2;
			};

			std::sort(m_globalList.begin(), m_globalList.end(), comparatorDescRaw);
			std::sort(m_handledWidgets.begin(), m_handledWidgets.end(), comparatorAsc);
		}

		std::vector<std::unique_ptr<Widget>> m_widgets;
		WidgetContainer<std::unique_ptr<Widget>> m_handledWidgets;
		std::vector<Widget*> m_addedSortedWidgets;
		WidgetContainer<Widget*> m_globalList;
	};


	template <class ... HL>
	class WidgetPanel : 
		public HandledWidgetTrait<WidgetPanel<HL...>, HL...>,
		public WidgetPanelSorter {
		using ParentTrait = HandledWidgetTrait<WidgetPanel<HL...>, HL...>;
	public:
		WidgetPanel() = default;

		explicit WidgetPanel(Widget& parent) :
			WidgetPanelSorter(parent) {
		}

		WidgetPanel(Widget& parent, const Point<int>& position) :
			WidgetPanelSorter(parent, position) {
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {
			/* If the current WidgetPanel doesn't accept the event, neither of his children do. */
			if (!ParentTrait::accept(e)) {
				return false;
			}

			 auto result = WidgetPanelSorter::notifyChildren(e);
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

		virtual ~WidgetPanel() = default;

	};
}
