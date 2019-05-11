#include <doctest.h>
#include "Core/Point.h"
#include "WindowsUtil.h"
#include "GUI/Events/ClickEventListener.h"
#include "GUI/Windows/DynamicWindowIG.h"
#include "Core/Window.h"

template<class ... T>
class DynamicWindowTest : public ska::DynamicWindowIG<T...> {
public:
	DynamicWindowTest(ska::Widget& parent, const ska::Rectangle& box) : ska::DynamicWindowIG<T...>(parent, box) {
	}

	DynamicWindowTest(ska::MouseObservable* mo, ska::KeyObservable* ko, const ska::Rectangle& box) : ska::DynamicWindowIG<T...>(mo, ko, box) {
	}

	virtual void render(ska::Renderer& r) const override {
		ska::DynamicWindowIG<T...>::render(r);
		m_callbackDisplay();
	}

	void whenDisplayed(std::function<void(void)> callback) {
		m_callbackDisplay = callback;
	}

private:
	std::function<void(void)> m_callbackDisplay;
};

class ObservableTest : public ska::MouseObservable, public ska::KeyObservable {

};


TEST_CASE("[DynamicWindow] Observable : clic") {
	ObservableTest obs;
	DynamicWindowTest<> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 });

	auto clicked = false;
	dw.addHandler<ska::ClickEventListener>([&](ska::Widget*, ska::ClickEvent&) {
		clicked = true;
	});

	SUBCASE("Interieur") {
        ska::Point<int> cePoint(0, 0);
        ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
        obs.ska::ClickObservable::notifyObservers(ce);

        CHECK(clicked);
	}

	SUBCASE("Exterieur") {
	    ska::Point<int> cePoint(100, 0);
        ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
        obs.ska::ClickObservable::notifyObservers(ce);

        CHECK(!clicked);
	}
}

TEST_CASE("[DynamicWindow] Observable : hover") {
	ObservableTest obs;
	DynamicWindowTest<> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 });

	auto hovered = false;
	dw.addHandler<ska::HoverEventListener>([&](ska::Widget*, ska::HoverEvent&) {
		hovered = true;
	});

	SUBCASE("Interieur") {
        ska::Point<int> hePoint(0, 0);
        ska::HoverEvent he(ska::MouseEventType::MOUSE_ENTER, hePoint, hePoint, hePoint);
        obs.ska::HoverObservable::notifyObservers(he);

        CHECK(hovered);
	}

	SUBCASE("Exterieur") {
        ska::Point<int> hePoint(0, 100);
        ska::HoverEvent he(ska::MouseEventType::MOUSE_ENTER, hePoint, hePoint, hePoint);
        obs.ska::HoverObservable::notifyObservers(he);

        CHECK(!hovered);
	}
}

TEST_CASE("[DynamicWindow] Observable : clavier") {
	ObservableTest obs;
	DynamicWindowTest<ska::KeyEventListener> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 });

	auto pressed = false;
	dw.addHandler<ska::KeyEventListener>([&](ska::Widget*, ska::KeyEvent&) {
		pressed = true;
	});

	ska::KeyEvent ke(ska::KeyEventType::KEY_DOWN, L"", 0);
	obs.ska::KeyObservable::notifyObservers(ke);

	CHECK(pressed);
}

TEST_CASE("[DynamicWindow] Suppression Observable : clic") {
	ObservableTest obs;
	auto clicked = false;
	{
        DynamicWindowTest<> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 });
        dw.addHandler<ska::ClickEventListener>([&](ska::Widget*, ska::ClickEvent&) {
            clicked = true;
        });
	}
	ska::Point<int> cePoint(0, 0);
	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
	obs.ska::ClickObservable::notifyObservers(ce);

	CHECK(!clicked);
}

TEST_CASE("[DynamicWindow] Suppression Observable : hover") {
	ObservableTest obs;
    auto hovered = false;
	{
        DynamicWindowTest<> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 });

        dw.addHandler<ska::HoverEventListener>([&](ska::Widget*, ska::HoverEvent&) {
            hovered = true;
        });
	}

	ska::Point<int> hePoint(0, 0);
	ska::HoverEvent he(ska::MouseEventType::MOUSE_ENTER, hePoint, hePoint, hePoint);
	obs.ska::HoverObservable::notifyObservers(he);

	CHECK(!hovered);
}

TEST_CASE("[DynamicWindow] Suppression Observable : clavier") {
	ObservableTest obs;
    auto pressed = false;
	{
        DynamicWindowTest<ska::KeyEventListener> dw(&obs, &obs, ska::Rectangle{ 0, 0, 10, 10 });

        dw.addHandler<ska::KeyEventListener>([&](ska::Widget*, ska::KeyEvent&) {
            pressed = true;
        });
	}

	ska::KeyEvent ke(ska::KeyEventType::KEY_DOWN, L"", 0);
	obs.ska::KeyObservable::notifyObservers(ke);

	CHECK(!pressed);
}
