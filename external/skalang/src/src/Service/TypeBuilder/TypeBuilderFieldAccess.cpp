#include "Config/LoggerConfigLang.h"
#include "TypeBuilderFieldAccess.h"

#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ScriptAST.h"
#include "Service/ScopedSymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>)

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FIELD_ACCESS>::build(const ScriptAST& script, OperateOn node) {
	const auto typeObject = node.GetObjectType();

	const auto& fieldName = node.GetFieldNameNode().name();

	// Built-in functions of maps : size(), has(key), keys(), get(key, default)
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
		// The value of the key, or the default value (of the type of the values) when the map has no such key
		if (fieldName == "get") {
			type.add(Type::MakeBuiltIn<ExpressionType::STRING>());
			type.add(typeObject[0]);
			type.add(typeObject[0]);
			return type;
		}
		auto ss = std::stringstream{};
		ss << "a map has no field \"" << fieldName << "\" (its functions : size(), has(key), keys(), get(key, default))";
		throw std::runtime_error(ss.str());
	}

	// Handling built-in direct fields
	if (typeObject == ExpressionType::ARRAY || typeObject == ExpressionType::STRING) {
		// contains(value) : true when an element of the array is equal to the value (elements compared with "==")
		if (fieldName == "contains" && typeObject == ExpressionType::ARRAY) {
			const auto& elementType = typeObject[0];
			if (!ExpressionTypeIsBuiltIn(elementType.type()) && elementType != ExpressionType::ENUM) {
				auto ss = std::stringstream{};
				ss << "contains(value) compares the elements with \"==\" : not available on an array of \"" << elementType << "\"";
				throw std::runtime_error(ss.str());
			}
			auto type = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
			type.add(elementType);
			type.add(Type::MakeBuiltIn<ExpressionType::BOOLEAN>());
			return type;
		}
		if (fieldName == "size") {
			auto type = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
			type.add(Type::MakeBuiltIn<ExpressionType::INT>());
			return type;
		}
		auto ss = std::stringstream{};
		ss << "trying to access an undeclared built-in field \"" << fieldName << "\" of the type \"" << typeObject << "\"";
		throw std::runtime_error(ss.str());
	}

	// The values of an enum : "Status.poison"
	if (typeObject == ExpressionType::ENUM) {
		if (typeObject.size() != 1 || typeObject.symbolTable() == nullptr) {
			auto ss = std::stringstream{};
			ss << "a value of an enum has no field (\"" << fieldName << "\")";
			throw std::runtime_error(ss.str());
		}
		const auto* valueTable = (*typeObject.symbolTable())(fieldName);
		if (valueTable == nullptr || valueTable->symbol() == nullptr) {
			auto names = std::string{};
			for (const auto& value : *typeObject.symbolTable()) {
				if (value != nullptr) {
					names += (names.empty() ? "" : ", ") + value->name();
				}
			}
			auto ss = std::stringstream{};
			ss << "\"" << fieldName << "\" is not a value of the enum \"" << typeObject.name() << "\" (its values : " << names << ")";
			throw std::runtime_error(ss.str());
		}
		return valueTable->symbol()->type();
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
