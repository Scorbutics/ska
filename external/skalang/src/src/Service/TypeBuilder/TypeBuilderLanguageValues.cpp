#include <set>
#include <sstream>
#include "Config/LoggerConfigLang.h"
#include "TypeBuilderLanguageValues.h"
#include "NodeValue/AST.h"
#include "Service/ScopedSymbolTable.h"

// ["k": v, ...] : a map of the type of its values, all of the same type, with string keys
ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::MAP_LITERAL>::build(const ScriptAST&, OperateOn node) {
	const auto& valueType = node[1].type().value();
	if (valueType == ExpressionType::VOID) {
		throw std::runtime_error("a value of a map literal has no type (void)");
	}
	for (std::size_t index = 0; index < node.size(); index += 2) {
		const auto& keyType = node[index].type().value();
		if (keyType != ExpressionType::STRING) {
			auto ss = std::stringstream{};
			ss << "a map key is a \"string\" (not a \"" << keyType << "\")";
			throw std::runtime_error(ss.str());
		}
		const auto& entryType = node[index + 1].type().value();
		if (entryType != valueType) {
			auto ss = std::stringstream{};
			ss << "map literal has not uniform types in it : \"" << valueType << "\" and \"" << entryType << "\"";
			throw std::runtime_error(ss.str());
		}
	}
	auto type = Type::MakeBuiltIn<ExpressionType::MAP>();
	type.add(valueType);
	return type;
}

// The type of the enum declaration : ENUM, with one compound type, the type of its values (a value is an ENUM without
// compound type). Both have the symbol table of the enum.
ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::ENUM_DECLARATION>::build(const ScriptAST&, OperateOn node) {
	const auto& valueType = node[0].type().value();
	auto type = Type::MakeBuiltIn(ExpressionType::ENUM, valueType.symbolTable());
	type.add(valueType);
	return { type, *valueType.symbolTable() };
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::MATCH_BRANCH>::build(const ScriptAST&, OperateOn node) {
	return node[0].type().value();
}

namespace ska {
	static bool IsMatchable(const Type& type) {
		return ExpressionTypeIsBuiltIn(type.type()) || (type == ExpressionType::ENUM && type.size() == 0);
	}
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::MATCH>::build(const ScriptAST&, OperateOn node) {
	const auto& subjectType = node[0].type().value();
	if (!IsMatchable(subjectType)) {
		auto ss = std::stringstream{};
		ss << "a match compares its value with \"==\" : not available on a \"" << subjectType << "\" (int, float, string, bool or the value of an enum)";
		throw std::runtime_error(ss.str());
	}

	const auto& resultType = node[1][0].type().value();
	if (resultType == ExpressionType::VOID) {
		throw std::runtime_error("a branch of a match gives a value (not void)");
	}

	auto hasElse = false;
	auto matchedNames = std::set<std::string>{};
	for (std::size_t index = 1; index < node.size(); index++) {
		const auto& branch = node[index];
		const auto& branchType = branch[0].type().value();
		if (branchType != resultType) {
			auto ss = std::stringstream{};
			ss << "the branches of a match give values of one type : \"" << resultType << "\" and \"" << branchType << "\"";
			throw std::runtime_error(ss.str());
		}
		hasElse = branch.size() == 1;
		for (std::size_t valueIndex = 1; valueIndex < branch.size(); valueIndex++) {
			const auto& value = branch[valueIndex];
			const auto& valueType = value.type().value();
			if (valueType != subjectType) {
				auto ss = std::stringstream{};
				ss << "a value of a match is a \"" << valueType << "\" but the matched value is a \"" << subjectType << "\"";
				throw std::runtime_error(ss.str());
			}
			// A value of an enum named in a branch ("Status.poison")
			if (subjectType == ExpressionType::ENUM && value.op() == Operator::FIELD_ACCESS && value.size() > 1) {
				matchedNames.insert(value[1].name());
			}
		}
	}

	if (!hasElse) {
		if (subjectType != ExpressionType::ENUM) {
			throw std::runtime_error("a match needs an \"else\" (the value when no branch matches), unless its branches name every value of an enum");
		}
		auto missing = std::string{};
		for (const auto& value : *subjectType.symbolTable()) {
			if (value != nullptr && matchedNames.find(value->name()) == matchedNames.end()) {
				missing += (missing.empty() ? "" : ", ") + value->name();
			}
		}
		if (!missing.empty()) {
			throw std::runtime_error("a match needs an \"else\" : the values of the enum \"" + subjectType.name() + "\" " + missing + " are not matched");
		}
	}

	return resultType;
}
