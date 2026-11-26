#include "Config/LoggerConfigLang.h"
#include <fstream>
#include "TypeBuilderUserDefinedObject.h"

#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::RETURN>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::USER_DEFINED_OBJECT>::build(const ScriptAST& script, OperateOn node) {
	auto& currentScope = script.symbols().current();
	const auto isScriptObject = currentScope.symbol() == nullptr || currentScope.symbol()->type() == ExpressionType::VOID && &script.symbols().current() == &script.symbols().current().parent();
	assert(currentScope.symbol() == nullptr || currentScope.symbol()->type() == ExpressionType::FUNCTION || isScriptObject);

	auto& previouslyDefinedSymbol = *script.symbols().current().back();

	return { ska::Type::MakeCustom<ExpressionType::OBJECT>(&previouslyDefinedSymbol), previouslyDefinedSymbol };
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::RETURN>::build(const ScriptAST& script, OperateOn node) {
	return { node.GetValue().type().value(), &node.GetValue() };
}
