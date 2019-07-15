#include <doctest.h>
#include "Script/__internalConfig/LoggerConfig.h"

#include "Core/ECS/EntityManager.h"

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

TEST_CASE("[ScriptComponent]") {
    auto data = ScriptComponentDataTest {};

    auto sc = ska::ScriptComponent{};

}
