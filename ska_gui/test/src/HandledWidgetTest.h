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
struct HandledWidgetTest : 
	public ska::Widget, 
	public ska::HandledWidgetTrait<HandledWidgetTest<EL...>, EL...>,
	public DisplayCounter {


	HandledWidgetTest() = default;

	HandledWidgetTest(ska::WidgetPanel& parent) : 
		ska::Widget(parent) {
	}

	HandledWidgetTest(ska::WidgetPanel& parent, ska::Point<int>& p) :
		ska::Widget(parent, p) {
	}
	
	bool notify(ska::IWidgetEvent& e) override { return ska::HandledWidgetTrait<HandledWidgetTest<EL...>, EL...>::tryTriggerHandlers(e); }

	void render(ska::Renderer& ) const override {
		if (ska::Widget::isVisible()) {
			DisplayCounter::pushVisible();
		} else {
			DisplayCounter::pushInvisible();
		}
	}

};
