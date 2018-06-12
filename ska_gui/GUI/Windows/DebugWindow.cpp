#include "DebugWindow.h"
#include "../GUI.h"
#include "../Components/Concrete/Label.h"
#include "../Components/Concrete/ImageRectangle.h"

ska::DebugWindow::DebugWindow(GUI& gui, GameEventDispatcher& ged) :
	SubObserver<ska::DebugGUIEvent>(std::bind(&DebugWindow::onDebugGUIEvent, this, std::placeholders::_1), ged),
	m_gui(gui),
	m_ged(ged) {
	m_dbgWindow = &m_gui.addWindow<ska::MoveableWindow<>>("DEBUG_WINDOW", ska::Rectangle{ 0, 0, 0, 0 });
	m_title = &m_dbgWindow->addWidget<ska::Label>("Debug", 11, ska::Point<int>{0, 0});
	m_title->setFontColor(255, 255, 255, 255);
	m_background = &m_dbgWindow->addWidget<ska::ImageRectangle>(Rectangle { 0, ska::WindowIG<>::TAILLEBLOCFENETRE / 2, 50, 50 }, ska::Color{ 220, 220, 220, OPACITY });
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
	w.modifyText(text);
	auto currentLabelWidth = w.getBox().w;
	if (currentLabelWidth > m_background->getBox().w) {
		const auto calculatedWidth = currentLabelWidth + 2 * HORIZONTAL_OFFSET;
		m_dbgWindow->setWidth(calculatedWidth);
		m_background->setWidth(calculatedWidth);
		m_title->move({ calculatedWidth / 2 - m_title->getBox().w / 2, m_title->getBox().y });
	}
}

bool ska::DebugWindow::onDebugGUIEvent(DebugGUIEvent& debugGuiEvent) {
	const auto& name = "dbg_window_" + debugGuiEvent.text;
	if (debugGuiEvent.type == DebugGUIEventType::ADD_DEBUG_INFO) {
		auto* label = &m_dbgWindow->addWidget<ska::Label>(debugGuiEvent.text + debugGuiEvent.refresher(), FONT_SIZE, ska::Point<int>{HORIZONTAL_OFFSET, static_cast<int>(m_labels.size() * (FONT_SIZE + 4) + VERTICAL_OFFSET + ska::WindowIG<>::TAILLEBLOCFENETRE / 2)});
		label->setFontColor(255, 0, 127, 255);
		m_labels.emplace(name, label);
		m_debugEntries.emplace(name, ska::DebugEntryLabel{ debugGuiEvent.text, std::move(debugGuiEvent.refresher), debugGuiEvent.delay });
	} else if (debugGuiEvent.type == DebugGUIEventType::REMOVE_DEBUG_INFO) {
		m_debugEntries.erase(name);
		if (m_labels.find(name) != m_labels.end()) {
			m_dbgWindow->removeWidget(m_labels.at(name));
			m_labels.erase(name);
		}
	}
	
	const auto calculatedHeight = m_labels.size() * (FONT_SIZE + 4) + 2 * VERTICAL_OFFSET;
	m_dbgWindow->setHeight(calculatedHeight);
	m_background->setHeight(calculatedHeight);
	return true;
}

