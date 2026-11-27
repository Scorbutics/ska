#include <doctest.h>
#include "LoggerConfig.h"
#include "GUI/Components/WidgetPanelInteractive.h"
#include "GUI/Components/Concrete/Image.h"
#include "GUI/Events/ClickEventListener.h"
#include "Graphic/Texture.h"

TEST_CASE("[Image] resized, it keeps the other textures of its file at their size") {
	// The textures of a file share one instance : an image shown smaller (ex : a Pokemon of the team in a slot) resizes its
	// own, not the one of the sprites made from the file after it
	const auto path = std::string{ "./resources/Menu/hpbar.png" };
	const auto before = ska::Texture{ path };
	REQUIRE(before.getWidth() == 69);
	ska::WidgetPanelInteractive<ska::ClickEventListener> panel;
	auto clip = ska::Rectangle{ 0, 0, 10, 8 };
	auto& image = panel.addWidget<ska::Image>(path, ska::Point<int>{}, false, &clip);
	CHECK(image.getBox().w == 10);
	CHECK(before.getWidth() == 69);
	CHECK(before.getHeight() == 15);
	const auto after = ska::Texture{ path };
	CHECK(after.getWidth() == 69);
	CHECK(after.getHeight() == 15);
}
