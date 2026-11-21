#include <doctest.h>
#include "Core/__internalConfig/LoggerConfig.h"
#include "Core/Draw/FrameClock.h"

TEST_CASE("[FrameClock] a value is drawn between its last two logic states") {
	auto value = ska::Interpolated{};
	ska::FrameClock::drawing(0.5F);
	// First drawing : no previous state
	CHECK(value.sample({ 10.F, 0.F, 0.F }, 96.F).x == doctest::Approx(10.F));

	ska::FrameClock::logicUpdated();
	ska::FrameClock::drawing(0.F);
	CHECK(value.sample({ 20.F, 0.F, 0.F }, 96.F).x == doctest::Approx(10.F));
	ska::FrameClock::drawing(0.5F);
	CHECK(value.sample({ 20.F, 0.F, 0.F }, 96.F).x == doctest::Approx(15.F));
	ska::FrameClock::drawing(1.F);
	CHECK(value.sample({ 20.F, 0.F, 0.F }, 96.F).x == doctest::Approx(20.F));

	SUBCASE("A jump is not smoothed") {
		ska::FrameClock::logicUpdated();
		ska::FrameClock::drawing(0.5F);
		CHECK(value.sample({ 500.F, 0.F, 0.F }, 96.F).x == doctest::Approx(500.F));
	}

	SUBCASE("A change between two logic updates is shown right away") {
		ska::FrameClock::logicUpdated();
		ska::FrameClock::drawing(0.5F);
		CHECK(value.sample({ 30.F, 0.F, 0.F }, 96.F).x == doctest::Approx(25.F));
		CHECK(value.sample({ 40.F, 0.F, 0.F }, 96.F).x == doctest::Approx(35.F));
	}
	ska::FrameClock::drawing(1.F);
}
