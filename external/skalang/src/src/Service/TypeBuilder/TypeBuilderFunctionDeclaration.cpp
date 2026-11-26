#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFunctionDeclaration.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FUNCTION_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto& prototype = node.GetFunctionPrototype();

	// Case where we return a new object
	if (node.GetFunctionBody().size() > 0 && node.GetFunctionBody().op() == Operator::BLOCK) {
		// Take the last body node as return
		auto& returnNode = node.GetFunctionBody()[node.GetFunctionBody().size() - 1];
		if (returnNode.symbolTable() != nullptr) {
			return { prototype.type().value(), *returnNode.symbolTable() };
		}
	}
	// Case where we return an existing object
	return prototype.type().value();
}
