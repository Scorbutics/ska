#include "Config/LoggerConfigLang.h"
#include "TypeBuilderParameterDeclaration.h"

#include "NodeValue/ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
    const auto& typeNode = node.GetTypeValueNode();
	return { typeNode.type().value(), &node.GetTypeValueNode() };
}

