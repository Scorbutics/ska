#include <doctest.h>
#include <fakeit.hpp>
#include <cassert>
#include "World/LayerEventLoader.h"
#include "World/LayerEvent.h"
#include "Exceptions/IllegalArgumentException.h"

ska::Vector2<ska::ScriptPack> BuildScriptPack(unsigned int width, unsigned int height) {
	assert(width > 1 && height > 3 && "Bad dimensions (too low)");
	auto pack = ska::Vector2<ska::ScriptPack> {};
	pack.resize(width, height);

	auto ssc = ska::ScriptSleepComponent{};
	ssc.id = { 47, 4 };
	ssc.triggeringType = ska::ScriptTriggerType::MOVE_IN;

	auto ssc2 = ska::ScriptSleepComponent{};
	ssc2.id = { 74122, 4 };
	ssc2.triggeringType = ska::ScriptTriggerType::TOUCH;

	pack[1][3].push_back(ssc);
	pack[1][3].push_back(ssc2);

	return std::move(pack);
}

TEST_CASE("[LayerEvent]") {

	static constexpr auto width = 15;
	static constexpr auto height = 16;

	auto eventLoader = fakeit::Mock<ska::LayerEventLoader>{}; ;

	auto sp = ska::ScriptGlobalPack{};
	auto ssc = ska::ScriptSleepComponent{};
	ssc.id = { 44, 47 };
	//Voluntarily set not an "AUTO" type, because it will be overriden by the event layer
	ssc.triggeringType = ska::ScriptTriggerType::TOUCH;
	sp.push_back({ ska::BlockEvent{}, ssc });

	Method(eventLoader, loadGlobal) = sp;
	Method(eventLoader, loadPositioned) = BuildScriptPack(width, height);
	Method(eventLoader, getName) = "LayerEventLoader name";


	auto layer = ska::LayerEvent{ eventLoader.get(), 14, 12 };

	SUBCASE("Auto scripts") {
		auto& scriptPack = layer.getAutoScript();
		CHECK(!scriptPack.empty());
		
		const auto checkId = scriptPack[0].scripts.id == ska::Point<int>{ 44, 47 };
		CHECK(checkId);

		const auto checkType = scriptPack[0].scripts.triggeringType == ska::ScriptTriggerType::AUTO;
		CHECK(checkType);
	}

	SUBCASE("Position scripts") {
		{
			auto& scriptPack = layer.getScripts({ 0, 4 });
			CHECK(scriptPack.empty());
		}

		auto& scriptPack2 = layer.getScripts({1, 3});
		CHECK(scriptPack2.size() == 2);

		{
			const auto checkId = scriptPack2[0].id == ska::Point<int>{ 47, 4 };
			CHECK(checkId);

			const auto checkType = scriptPack2[0].triggeringType == ska::ScriptTriggerType::MOVE_IN;
			CHECK(checkType);
		}
		{
			const auto checkId = scriptPack2[1].id == ska::Point<int>{ 74122, 4 };
			CHECK(checkId);

			const auto checkType = scriptPack2[1].triggeringType == ska::ScriptTriggerType::TOUCH;
			CHECK(checkType);
		}

	}

}
