#include <doctest.h>
#include <fakeit.hpp>
#include "World/TilesetEvent.h"

TEST_CASE("[TilesetEvent]") {
	auto eventLoader = fakeit::Mock<ska::TilesetEventLoader>{};	

	SUBCASE("TilesetEvent without scripts") {
		Method(eventLoader, load) = std::vector<ska::ScriptSleepComponent>{ };
		auto tileset = ska::TilesetEvent{ "TilesetEvent name", eventLoader.get() };
		
		const auto scripts = tileset.getScript(ska::ScriptTriggerType::TOUCH, { 5, 7 });
		CHECK(scripts.empty());
	}

	SUBCASE("TilesetEvent with non auto scripts") {
		auto ssc1 = ska::ScriptSleepComponent{};
		ssc1.context = "test context";
		ssc1.deleteEntityWhenFinished = true;
		ssc1.id = { 0, 3 };
		ssc1.name = "TilesetEvent.test";
		ssc1.period = 1000;
		ssc1.triggeringType = ska::ScriptTriggerType::TOUCH;

		auto ssc2 = ska::ScriptSleepComponent{};
		ssc2.id = { 5, 7 };
		ssc2.name = "TilesetEvent name";
		ssc2.triggeringType = ska::ScriptTriggerType::TOUCH;

		auto ssc3 = ska::ScriptSleepComponent{};
		ssc3.id = { 5, 7 };
		ssc3.name = "TilesetEvent name";
		ssc3.triggeringType = ska::ScriptTriggerType::MOVE_IN;

		Method(eventLoader, load) = std::vector<ska::ScriptSleepComponent>{ ssc1, ssc2, ssc3 };
		auto tileset = ska::TilesetEvent{ "TilesetEvent name", eventLoader.get() };

		SUBCASE("Find Scripts") {
			const auto scripts = tileset.getScript(ska::ScriptTriggerType::TOUCH, { 5, 7 });
			CHECK(scripts.size() == 1);

			const auto scripts2 = tileset.getScript(ska::ScriptTriggerType::MOVE_IN, { 5, 7 });
			CHECK(scripts.size() == 1);
		}
	}

	SUBCASE("TilesetEvent with auto scripts") {
		auto ssc1 = ska::ScriptSleepComponent{};
		ssc1.deleteEntityWhenFinished = true;
		ssc1.id = { 0, 3 };
		ssc1.name = "TilesetEvent.test";
		ssc1.triggeringType = ska::ScriptTriggerType::AUTO;

		auto ssc2 = ska::ScriptSleepComponent{};
		ssc2.id = { 5, 7 };
		ssc2.name = "TilesetEvent name";
		ssc2.triggeringType = ska::ScriptTriggerType::TOUCH;

		auto ssc3 = ska::ScriptSleepComponent{};
		ssc3.id = { 5, 7 };
		ssc3.name = "TilesetEvent name";
		ssc3.triggeringType = ska::ScriptTriggerType::MOVE_IN;

		Method(eventLoader, load) = std::vector<ska::ScriptSleepComponent>{ ssc1, ssc2, ssc3 };
		auto tileset = ska::TilesetEvent{ "TilesetEvent name", eventLoader.get() };

		SUBCASE("Find Auto Scripts") {
			const auto scripts = tileset.getScript(ska::ScriptTriggerType::AUTO, { 5, 7 });
			CHECK(scripts.size() == 1);
			CHECK(scripts[0].get().id == ssc1.id);
			CHECK(scripts[0].get().name == ssc1.name);

			const auto scripts2 = tileset.getScript(ska::ScriptTriggerType::AUTO);
			CHECK(scripts2.size() == 1);
			CHECK(scripts2[0].get().id == ssc1.id);
			CHECK(scripts2[0].get().name == ssc1.name);
		}
	}

}
