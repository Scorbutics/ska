#include "DebugWindow.h"
#include "../GUI.h"
#include "../Components/Concrete/Label.h"
#include "../Components/Concrete/ImageRectangle.h"
#include "../Components/Concrete/TileSurface.h"

ska::DebugWindow::DebugWindow(GUI& gui, GameEventDispatcher& ged) :
	SubObserver<ska::DebugGUIEvent>(std::bind(&DebugWindow::onDebugGUIEvent, this, std::placeholders::_1), ged),
	m_gui(gui),
	m_ged(ged) {
	m_dbgWindow = &m_gui.addWindow<ska::MoveableWindow<>>("DEBUG_WINDOW", ska::Rectangle{ 10, 10, 1, 1 });
	m_dbgWindow->setTitle("Debug");
	auto& tileSurface = m_dbgWindow->setBackground<ska::TileSurface>(Rectangle{ 0, 0, 2, 2 }, ska::GUI::MENU_DEFAULT_THEME_PATH + "menu.png");
	tileSurface.setOpacity(OPACITY);
}

void ska::DebugWindow::refresh(unsigned int ellapsedTime) {
	for (auto& debugEntry : m_debugEntries) {
		auto& debug = debugEntry.second;
		debug.count += ellapsedTime;
		if (debug.count > debug.delay) {
			debug.count = 0;
			auto& w = reinterpret_cast<ska::Label&>(*m_labels["dbg_window_" + debug.label]);
			editText(w, std::move(debug.label + debug.updater()));
		}
	}
}

void ska::DebugWindow::editText(ska::Label& w, std::string text) {
	w.modifyText(std::move(text));
	auto currentLabelWidth = w.getBox().w;
	if (currentLabelWidth > m_dbgWindow->getBox().w) {
		const auto calculatedWidth = currentLabelWidth + 2 * HORIZONTAL_OFFSET;
		m_dbgWindow->setWidth(calculatedWidth);
	}
}

bool ska::DebugWindow::onDebugGUIEvent(DebugGUIEvent& debugGuiEvent) {
	const auto& name = "dbg_window_" + debugGuiEvent.text;
	if (debugGuiEvent.type == DebugGUIEventType::ADD_DEBUG_INFO) {
		if (debugGuiEvent.refresher) {
			auto* label = &m_dbgWindow->addWidget<ska::Label>(" ", FONT_SIZE, ska::Point<int>{HORIZONTAL_OFFSET, static_cast<int>(m_labels.size() * (FONT_SIZE + 4) + VERTICAL_OFFSET + 16)});
			label->setFontColor(255, 0, 127, 255);
			m_labels.emplace(name, label);
			const auto labelText = debugGuiEvent.text + debugGuiEvent.refresher();
			m_debugEntries.emplace(name, ska::DebugEntryLabel{ debugGuiEvent.text, std::move(debugGuiEvent.refresher), debugGuiEvent.delay });
			const auto calculatedHeight = m_labels.size() * (FONT_SIZE + 4) + 2 * VERTICAL_OFFSET + 16;
			m_dbgWindow->setHeight(calculatedHeight);
			editText(*label, labelText);
		}
		return true;
	} 
	
	if (debugGuiEvent.type == DebugGUIEventType::REMOVE_DEBUG_INFO) {
		m_debugEntries.erase(name);
		if (m_labels.find(name) != m_labels.end()) {
			m_dbgWindow->removeWidget(m_labels.at(name));
			m_labels.erase(name);
		}
		const auto calculatedHeight = m_labels.size() * (FONT_SIZE + 4) + 2 * VERTICAL_OFFSET + 16;
		m_dbgWindow->setHeight(calculatedHeight);
		return true;
	}
	
	return false;
}

