#include "Config/LoggerConfigLang.h"
#include "TypeBuilderType.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::TYPE>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::TYPE>::build(const ScriptAST& script, OperateOn node) {
	auto result = Type{};
	const ScopedSymbolTable* resultSymbolTable = nullptr;
	const auto isBuiltIn = node.IsBuiltIn();
    if (!isBuiltIn) {
		auto typeSymbolTable = node.GetTypeSymbol(script.symbols());
		if (typeSymbolTable != nullptr && typeSymbolTable->symbol() != nullptr) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node looked \"" << typeSymbolTable->symbol()->type() << "\" for node \"" << node.GetName() << "\"";
			resultSymbolTable = typeSymbolTable;
			if (node.IsObject()) {
				result = Type::MakeCustom<ExpressionType::OBJECT>(resultSymbolTable);
			} else {
				result = resultSymbolTable->symbol()->type();
			}
		} else if (node.IsArray()) {
			result = node.GetType();
		} else {
			throw std::runtime_error("undeclared custom type \"" + node.GetName() + "\" (when trying to look on token type \"" + node.GetTypeName() + "\")");
		}

   } else {
		assert(!node.IsObject());
	   result = ExpressionTypeMap.at(node.GetName());
	   SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Type-node is a built-in : \"" << node.GetName() << "\"";
   }

	//handles array and map types
	if (node.IsMap()) {
		result = Type::MakeBuiltIn<ExpressionType::MAP>().add(result);
	} else if (node.IsArray()) {
		result = Type::MakeBuiltIn<ExpressionType::ARRAY>().add(result);
	}

	SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::TYPE>) << "Resulting type : \"" << result << "\"";

	return resultSymbolTable == nullptr ? TypeHierarchy{ result } : TypeHierarchy{result, *resultSymbolTable};
}
