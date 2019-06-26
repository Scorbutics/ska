#pragma once
#include <vector>
#include "GUI/Components/HandledWidget.h"

class DisplayCounter {
public:
	DisplayCounter() {
		reset();
	}

	static auto& getInstances() {
		static std::vector<const DisplayCounter*> instances;
		return instances;
	}

	static auto& getDisplayedInstances() {
		static std::vector<const DisplayCounter*> displayedInstances;
		return displayedInstances;
	}

	static void reset() {
		getDisplayedInstances().clear();
		getInstances().clear();
	}

	virtual ~DisplayCounter() {  }

protected:
	void pushVisible() const {
		getDisplayedInstances().push_back(this);
		pushInvisible();
	}

	void pushInvisible() const{
		getInstances().push_back(this);
	}

};

template <class ... EL>
struct HandledWidgetTest : public ska::HandledWidget<EL...>, public DisplayCounter {
	HandledWidgetTest() {
	}

	HandledWidgetTest(ska::Widget& parent) : ska::HandledWidget<EL...>(parent) {
	}

	HandledWidgetTest(ska::Widget& parent, ska::Point<int>& p) : ska::HandledWidget<EL...>(parent, p) {
	}

	void render(ska::Renderer& ) const override {
		if (ska::HandledWidget<EL...>::isVisible()) {
			DisplayCounter::pushVisible();
		} else {
			DisplayCounter::pushInvisible();
		}
	}

};
