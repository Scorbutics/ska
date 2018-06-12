#include "DebugWindow.h"
#include "../GUI.h"
#include "../Components/Concrete/Label.h"
#include "../Components/Concrete/ImageRectangle.h"

ska::DebugWindow::DebugWindow(GUI& gui, GameEventDispatcher& ged) :
	SubObserver<ska::DebugGUIEvent>(std::bind(&DebugWindow::onDebugGUIEvent, this, std::placeholders::_1), ged),
	m_gui(gui),
	m_ged(ged) {
	m_dbgWindow = &m_gui.addWindow<ska::WindowIG<>>("DEBUG_WINDOW", ska::Rectangle{ 0 }, "");
	m_dbgWindow->addWidget<ska::ImageRectangle>(Rectangle { 0, 0, 200, 70 }, ska::Color{ 220, 220, 220, 80 });
}

void ska::DebugWindow::refresh(unsigned int ellapsedTime) {
	for (auto& debugEntry : m_debugEntries) {
		auto& debug = debugEntry.second;
		debug.count += ellapsedTime;
		if (debug.count > debug.delay) {
			debug.count = 0;
			auto& w = reinterpret_cast<ska::Label&>(*m_labels["dbg_window_" + debug.label]);
			const auto newText = debug.updater();
			w.modifyText(debug.label + newText);
		}
	}
}

bool ska::DebugWindow::onDebugGUIEvent(DebugGUIEvent& debugGuiEvent) {
	const auto& name = "dbg_window_" + debugGuiEvent.text;
	if (debugGuiEvent.type == DebugGUIEventType::ADD_DEBUG_INFO) {
		m_debugEntries.emplace(name, ska::DebugEntryLabel{ debugGuiEvent.text, std::move(debugGuiEvent.refresher), debugGuiEvent.delay });
		auto* label = &m_dbgWindow->addWidget<ska::Label>("", 14, ska::Point<int>{0, static_cast<int>(m_labels.size() * 18)});
		label->setFontColor(255, 0, 127, 255);
		m_labels.emplace(name, label);
	} else if (debugGuiEvent.type == DebugGUIEventType::REMOVE_DEBUG_INFO) {
		m_debugEntries.erase(name);
		if (m_labels.find(name) != m_labels.end()) {
			m_dbgWindow->removeWidget(m_labels.at(name));
			m_labels.erase(name);
		}
	}
	return true;
}

