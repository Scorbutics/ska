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
		// ["k": v, ...], a map with its entries (children : key, value, key, value...)
		MAP_LITERAL,
		// enum a, b end (children : the names of the values)
		ENUM_DECLARATION,
		// match subject when v, w then x ... else y end (children : subject, branches, optional else value)
		MATCH,
		// when v, w then x (children : the result x, then the values v, w)
		MATCH_BRANCH,
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
		"MAP_DECLARATION",
		"MAP_LITERAL",
		"ENUM_DECLARATION",
		"MATCH",
		"MATCH_BRANCH"
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
