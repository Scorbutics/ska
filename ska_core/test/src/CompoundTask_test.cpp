#include <doctest.h>
#include "Base/Task/CompoundTask.h"

TEST_CASE("[CompoundTask]"){
	using TaskPtr = std::unique_ptr<ska::Task>;
	auto t1Done = false;
	TaskPtr t1 = std::make_unique<ska::Task>([&](ska::Task&) {
		t1Done = true;
		return false;
	});

	auto t2Done = false;
	TaskPtr t2 = std::make_unique<ska::Task>([&](ska::Task&) {
		t2Done = true;
		return false;
	});

	ska::CompoundTask ct(std::move(t1), std::move(t2));

	SUBCASE("Simple result") {
		auto result = ct();
		CHECK(t1Done);
		CHECK(t2Done);
		CHECK(!result);
	}
}