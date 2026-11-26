#pragma once

#include <memory>
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/ReservedKeywordsPool.h"

#include "Generator/BytecodeGenerator.h"
#include "Generator/Value/BytecodeScriptCache.h"

#include "BytecodeInterpreter/BytecodeInterpreter.h"

struct BytecodeInterpreterDataTestContainer {
	using ParserPtr = std::unique_ptr<ska::StatementParser>;
	using SemanticTypeCheckerPtr = std::unique_ptr<ska::SemanticTypeChecker>;
	using TypeBuilderPtr = std::unique_ptr<ska::TypeBuilder>;
	using BytecodeGeneratorPtr = std::unique_ptr<ska::bytecode::Generator>;
	using BytecodeInterpreterPtr = std::unique_ptr<ska::bytecode::Interpreter>;

	ska::ReservedKeywordsPool reservedKeywords;
	BytecodeGeneratorPtr generator;
	ParserPtr parser;
	SemanticTypeCheckerPtr typeChecker;
	TypeBuilderPtr typeBuilder;
	BytecodeInterpreterPtr interpreter;
	std::unique_ptr<ska::bytecode::ScriptCache> storage = std::make_unique<ska::bytecode::ScriptCache>();
};
