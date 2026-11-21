#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const ScriptAST& script, OperateOn node) {
	const auto typeObject = node.GetObjectType();

	const auto& fieldName = node.GetFieldNameNode().name();

	// Built-in functions of maps : size(), has(key), keys()
	if (typeObject == ExpressionType::MAP) {
		auto type = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
		if (fieldName == "size") {
			type.add(Type::MakeBuiltIn<ExpressionType::INT>());
			return type;
		}
		if (fieldName == "has") {
			type.add(Type::MakeBuiltIn<ExpressionType::STRING>());
			type.add(Type::MakeBuiltIn<ExpressionType::BOOLEAN>());
			return type;
		}
		if (fieldName == "keys") {
			type.add(Type::MakeBuiltIn<ExpressionType::ARRAY>().add(Type::MakeBuiltIn<ExpressionType::STRING>()));
			return type;
		}
		auto ss = std::stringstream{};
		ss << "a map has no field \"" << fieldName << "\" (its functions : size(), has(key), keys())";
		throw std::runtime_error(ss.str());
	}

	// Handling built-in direct fields
	if (typeObject == ExpressionType::ARRAY || typeObject == ExpressionType::STRING) {
		if (fieldName == "size") {
			auto type = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
			type.add(Type::MakeBuiltIn<ExpressionType::INT>());
			return type;
		}
		auto ss = std::stringstream{};
		ss << "trying to access an undeclared built-in field \"" << fieldName << "\" of the type \"" << typeObject << "\"";
		throw std::runtime_error(ss.str());
	}

	const auto* symbolObjectTable = node.GetObjectSymbolTable();

	// Handling custom objects
	if (symbolObjectTable == nullptr || typeObject != ExpressionType::OBJECT) {
		auto error = std::stringstream {};
		if (typeObject == ExpressionType::VOID) {
			error << "the class symbol table \"" << node.GetObjectNameNode().name() << "\" is not registered. Maybe you're trying to use the type you're defining in its definition...";
		} else {
			error << "the variable \"" << node.GetObjectNameNode().name() << "\" is not registered as an object but as a \"" << typeObject << "\" type";
		}
		throw std::runtime_error(error.str());
	}

	const auto* symbolFieldTable = (*symbolObjectTable)(fieldName);
	if (symbolFieldTable == nullptr || symbolFieldTable->symbol() == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
        throw std::runtime_error(ss.str());
    }

    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>) << "Field accessed \"" << fieldName << "\" (type \"" << symbolFieldTable->symbol()->type() << "\") of \"" << node.GetObjectNameNode().name() << "\"";

	if (symbolFieldTable->symbol()->type() == ExpressionType::VOID) {
		auto ss = std::stringstream{};
		ss << "field \"" << node.GetFieldNameNode().name() << "\" of \"" << node.GetObjectNameNode().name() << "\" has a void type, which is invalid";
		throw std::runtime_error(ss.str());
	}

	return { *symbolFieldTable };
}
