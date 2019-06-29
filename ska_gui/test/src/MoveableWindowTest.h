#pragma once
#include "GUI/Windows/MoveableWindow.h"

template<class ... T>
class MoveableWindowTest : public ska::MoveableWindow<T...> {
public:
	MoveableWindowTest(ska::WidgetPanel& parent, const ska::Rectangle& box, const std::string& styleName) : ska::MoveableWindow<T...>(parent, box) {
	}

	MoveableWindowTest(ska::MouseObservable& mo, ska::KeyObservable& ko, const ska::Rectangle& box) : ska::MoveableWindow<T...>(mo, ko, box) {
	}

	virtual void render(ska::Renderer& r) const override {
		ska::MoveableWindow<T...>::render(r);
		m_callbackDisplay();
	}

	void whenDisplayed(std::function<void(void)> callback) {
		m_callbackDisplay = callback;
	}

private:
	std::function<void(void)> m_callbackDisplay;
};
