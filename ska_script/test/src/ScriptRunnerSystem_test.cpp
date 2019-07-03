#include <doctest.h>
#include "Script/__internalConfig/LoggerConfig.h"
#include "Script/System/ScriptRunnerSystem.h"

struct ScriptRunnerSystemDataTest {
	ska::GameEventDispatcher ged {};
	ska::EntityManager entityManager { ged };
	ska::ScriptRunnerSystem scriptRunnerSystem { entityManager, ged };
};

TEST_CASE("[ScriptRunnerSystem]") {
	auto data = ScriptRunnerSystemDataTest {};

	auto ssc = ska::ScriptSleepComponent {};
	ssc.id = ska::Point<int>{ 0, 0 };
	ssc.name = "ska_script_test";

	ssc.triggeringType = ska::ScriptTriggerType::AUTO;
	ssc.period = 1000;

	auto basicScriptEntity = data.entityManager.createEntity();
	auto scriptTriggererEntity = data.entityManager.createEntity();
	//data.entityManager.addComponent<ska::ScriptSleepComponent>(basicEntity, std::move(ssc));

	SUBCASE("[ScriptRunnerSystem] Registering ska_script_test script") {
    data.scriptRunnerSystem.registerScript(ssc, scriptTriggererEntity);

		SUBCASE("[ScriptRunnerSystem] Registering another script") {
			auto ssc2 = ska::ScriptSleepComponent {};
			ssc2.id = ska::Point<int>{ 0, 0 };
			ssc2.name = "ska_script_test2";

			ssc2.triggeringType = ska::ScriptTriggerType::AUTO;
			ssc2.period = 1000;
			data.scriptRunnerSystem.registerScript(ssc2, scriptTriggererEntity);
		}

		SUBCASE("[ScriptRunnerSystem] Update script system") {
			data.scriptRunnerSystem.update(16);
		}
	}

}
