#include "../Events/StopType.h"
#include "../Events/IWidgetEvent.h"
#include "WidgetPanel.h"

void ska::WidgetPanel::render(Renderer& renderer) const {
	if (!isVisible()) {
		return;
	}
	for (auto& w : m_globalList) {
		if (w->isVisible()) {
			w->render(renderer);
		} else {
			//First invisible widget found : we stop
			break;
		}
	}
}

ska::Widget* ska::WidgetPanel::backAddedWidget() {
	return m_addedSortedWidgets.empty() ? nullptr : m_addedSortedWidgets.back();
}

ska::Widget* ska::WidgetPanel::backWidget() {
	return m_globalList.empty() ? nullptr : m_globalList.back();
}

void ska::WidgetPanel::clear() {
	m_widgets.clear();
	m_handledWidgets.clear();
	m_globalList.clear();
	m_addedSortedWidgets.clear();
}

ska::Widget* ska::WidgetPanel::getWidget(std::size_t index) {
	return m_addedSortedWidgets[index];
}

bool ska::WidgetPanel::notifyChildren(IWidgetEvent& e) {
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

void ska::WidgetPanel::resort() {
	organizeHandledWidgets();
	sortZIndexWidgets();
}

void ska::WidgetPanel::organizeHandledWidgets() {
	std::size_t cursor = 0;
	for (auto& w : m_handledWidgets) {
		m_handledWidgets.organize(w, cursor);
		cursor++;
	}
}

void ska::WidgetPanel::sortZIndexWidgets() {
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
