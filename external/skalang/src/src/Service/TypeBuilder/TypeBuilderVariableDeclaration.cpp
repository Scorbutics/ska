#include "Config/LoggerConfigLang.h"
#include "TypeBuilderVariableDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"
#include "TypeBuilderCalculatorDispatcher.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::DECLARATION>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::DECLARATION>::build(const ScriptAST& script, OperateOn node) {
    const auto* symbolTable = script.symbols()[node.GetVariableName()];
    if (symbolTable != nullptr && symbolTable->symbol() != nullptr && symbolTable->symbol()->type() != ExpressionType::VOID) {
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::DECLARATION>) << "%02cVariable declaration has already type \"" << symbolTable->symbol()->type() << "\"";
	    return symbolTable->symbol()->type();
    }

    auto resultType = node.GetVariableValueNode().type().value();
    const auto* nodeValue = ExpressionTypeIsBuiltIn(resultType.type()) ? nullptr : &node.GetVariableValueNode();
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::DECLARATION>) << "%02cVariable declaration type \"" << resultType << "\"";
    if (nodeValue != nullptr) {
        SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::DECLARATION>) << " for node \"" << (*nodeValue) << "\"";
    }
    return { std::move(resultType), nodeValue };
}
