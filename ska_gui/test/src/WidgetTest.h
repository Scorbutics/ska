#pragma once
#include "GUI/Components/Widget.h"

struct WidgetTest : public ska::Widget {
	WidgetTest() {
	}

	WidgetTest(ska::WidgetPanel& parent) : ska::Widget(parent) {
	}

	WidgetTest(ska::WidgetPanel& parent, ska::Point<int>& p) : ska::Widget(parent, p) {
	}

	void render(ska::Renderer& renderer) const override {}
	bool notify(ska::IWidgetEvent& e) override { return false; }
};
