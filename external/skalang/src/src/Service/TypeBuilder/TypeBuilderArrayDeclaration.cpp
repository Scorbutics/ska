#include "Config/LoggerConfigLang.h"
#include "TypeBuilderArrayDeclaration.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "TypeBuilderCalculatorDispatcher.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::ARRAY_TYPE_DECLARATION>);
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>);

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	const auto* arraySymbol = node.GetArraySymbol();
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::ARRAY_DECLARATION>) << "%02cArray declaration symbol : \"" << (arraySymbol == nullptr ? "" : arraySymbol->name()) << "\"";
	if (!node.HasArrayType()) {
		return Type::MakeBuiltIn(ExpressionType::VOID);
	}
	auto type = Type::MakeBuiltIn<ExpressionType::ARRAY>();
	const auto& subType = node.GetArraySubType();
	type.add(subType);
	return { type, node.GetArrayNode() };
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ARRAY_TYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	const auto* arraySymbol = node.GetArraySymbol();
	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::ARRAY_TYPE_DECLARATION>) << "%02cArray type declaration symbol : \"" << (arraySymbol == nullptr ? "" : arraySymbol->name()) << "\"";
	if (!node.HasExplicitArrayType()) {
		return { node.GetArrayContent().type().value(), &node.GetArrayContent() };
	}
	auto type = Type::MakeBuiltIn<ExpressionType::ARRAY>();
	const auto& subType = node.GetArraySubType();
	type.add(subType);
	return { type, &node.GetArrayContent() };
}
