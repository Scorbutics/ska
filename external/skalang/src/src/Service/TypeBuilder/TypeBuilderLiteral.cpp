#include "Config/LoggerConfigLang.h"
#include "TypeBuilderLiteral.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::LITERAL>);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::UNARY>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::LITERAL>::build(const ScriptAST& script, OperateOn node) {
	auto& symbols = script.symbols();
	return TypeBuilderBuildFromTokenType(symbols, node.asNode());
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::UNARY>::build(const ScriptAST& script, OperateOn node) {
	auto& symbols = script.symbols();
	return TypeBuilderBuildFromTokenType(symbols, node.asNode());
}
