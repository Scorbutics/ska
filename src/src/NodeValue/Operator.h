#pragma once
#include <cstddef>
#include <ostream>

namespace ska {
	enum class Operator {
		BINARY,
    	UNARY,
		LITERAL,
		DECLARATION,
		AFFECTATION,
		PARAMETER_DECLARATION,
		FILTER_DECLARATION,
		FILTER_PARAMETER_DECLARATION,
		FILTER,
		ARRAY_TYPE_DECLARATION,
		ARRAY_DECLARATION,
		ARRAY_USE,
    	USER_DEFINED_OBJECT,
    	FUNCTION_CALL,
		FUNCTION_PROTOTYPE_DECLARATION,
		FUNCTION_DECLARATION,
		FACTORY_PROTOTYPE_DECLARATION,
		CONVERTER_CALL,
		FIELD_ACCESS,
		FOR_LOOP,
		IMPORT,
		EXPORT,
		BLOCK,
		IF,
		IF_ELSE,
		RETURN,
		SCRIPT_LINK,
		SCRIPT_OBJECT,
		TYPE,
		// [:]: T, an empty map
		MAP_DECLARATION,
		UNUSED_Last_Length
	};

	static constexpr const char* OperatorSTR[] = {
		"BINARY",
    	"UNARY",
		"LITERAL",
		"DECLARATION",
		"AFFECTATION",
		"PARAMETER_DECLARATION",
		"FILTER_DECLARATION",
		"FILTER_PARAMETER_DECLARATION",
		"FILTER",
		"ARRAY_TYPE_DECLARATION",
		"ARRAY_DECLARATION",
		"ARRAY_USE",
    	"USER_DEFINED_OBJECT",
    	"FUNCTION_CALL",
		"FUNCTION_PROTOTYPE_DECLARATION",
		"FUNCTION_DECLARATION",
		"FACTORY_PROTOTYPE_DECLARATION",
		"CONVERTER_CALL",
		"FIELD_ACCESS",
		"FOR_LOOP",
		"IMPORT",
		"EXPORT",
		"BLOCK",
		"IF",
		"IF_ELSE",
		"RETURN",
		"SCRIPT_LINK",
		"SCRIPT_OBJECT",
		"TYPE",
		"MAP_DECLARATION"
	};

	static_assert(sizeof(OperatorSTR) / sizeof(OperatorSTR[0]) == static_cast<std::size_t>(Operator::UNUSED_Last_Length));

	static inline const char* OperatorPrint(Operator op) {
    	return OperatorSTR[static_cast<std::size_t>(op)];
	}

	inline std::ostream& operator<<(std::ostream& stream, const Operator& op) {
		stream << ska::OperatorPrint(op);
		return stream;
	}
}
