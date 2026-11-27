#pragma once

#include <string>
#include <vector>
#include "GeneratorOperator.h"
#include "Operation/OperationTypeMapLiteral.h"
#include "Operation/OperationTypeEnumDeclaration.h"
#include "Operation/OperationTypeMatch.h"

namespace ska {
	class ScopedSymbolTable;
	namespace bytecode {
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::MAP_LITERAL);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::ENUM_DECLARATION);
		SKALANG_GENERATOR_OPERATOR_DEFINE(Operator::MATCH);

		// The names of the values of an enum (its symbol table), in their declaration order
		std::vector<std::string> EnumValueNames(const ScopedSymbolTable& enumTable);
		// A string array of these names, in a new register
		InstructionOutput GenerateStringArray(const std::vector<std::string>& names, GenerationContext& context);
		// A string constant
		Operand StringConstant(const std::string& text);
	}
}
