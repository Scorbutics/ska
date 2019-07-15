#include <doctest.h>
#include "Script/__internalConfig/LoggerConfig.h"

#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"

//skalang
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Interpreter/Interpreter.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Script/ScriptComponent.h"
#include "Interpreter/ScriptCache.h"
#include "std/module.h"

struct ScriptComponentDataTest {
    const ska::ReservedKeywordsPool reservedKeywords;
    ska::ScriptCache scriptCache;
    ska::TypeCrosser typeCrosser;
    ska::StatementParser parser{reservedKeywords};
    ska::TypeBuilder typeBuilder{parser, typeCrosser};
    ska::SymbolTableTypeUpdater symbolsTypeUpdater{parser};
    ska::SemanticTypeChecker typeChecker{parser, typeCrosser};
    ska::Interpreter interpreter{reservedKeywords, typeCrosser};
    ska::lang::ModuleConfiguration moduleConfiguration{scriptCache, typeBuilder, symbolsTypeUpdater, reservedKeywords, parser, interpreter};
};

inline auto BuildScript(ScriptComponentDataTest& data, int period = 0, ska::ScriptTriggerType type = ska::ScriptTriggerType::AUTO) {
    auto ss = std::stringstream {};
    static unsigned int count = 0;
    ss << count++;
	auto executor = std::make_unique<ska::Script>(data.scriptCache, "executor_sc_" + ss.str(), 
		ska::Tokenizer{ data.reservedKeywords,
			"var toto = 0;"
    }.tokenize());

    executor->parse(data.parser);

    auto scriptData = ska::ScriptSleepComponent{};
    scriptData.name = "sc_" + ss.str();
    scriptData.period = period;
    scriptData.triggeringType = type;
    return ska::ScriptComponent(scriptData, 0, 1, std::move(executor));
}

TEST_CASE("[ScriptComponent] Priority grows over time") {
    auto data = ScriptComponentDataTest {};
    auto sc = BuildScript(data);
    auto p = sc.getPriority(15);
    auto p2 = sc.getPriority(145);
    CHECK(p2 > p);
}

TEST_CASE("[ScriptComponent] Priority depends on period (bigger period = less priority)") {
    auto data = ScriptComponentDataTest {};
    auto sc = BuildScript(data, 17);
    auto sc2 = BuildScript(data, 9);

    auto p = sc.getPriority(1);
    auto p2 = sc2.getPriority(1);
    CHECK(p2 > p);
}

TEST_CASE("[ScriptComponent] Priority depends on script state (a PAUSED script grows faster)") {
    auto data = ScriptComponentDataTest {};
    auto sc = BuildScript(data, 1, ska::ScriptTriggerType::TOUCH);
    auto sc2 = BuildScript(data, 1, ska::ScriptTriggerType::AUTO);

    sc.play(data.interpreter);
    sc2.play(data.interpreter);

    auto p = sc.getPriority(10);
    auto p2 = sc2.getPriority(10);
    CHECK(p2 > p);
}


