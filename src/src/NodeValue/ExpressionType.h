#pragma once
#include <unordered_map>

namespace ska {
	struct Type;
}

namespace ska {

	enum class ExpressionType {
		VOID,
		OBJECT,
		FUNCTION,
    	INT,
		FLOAT,
		STRING,
		ARRAY,
		BOOLEAN,
		// string keys, values of one type
		MAP,
		UNUSED_Last_Length
	};

	static constexpr const char* ExpressionTypeSTR[] = {
		"void",
		"var",
		"function",
    	"int",
		"float",
		"string",
		"array",
		"bool",
		"map"
	};

	static_assert(sizeof(ExpressionTypeSTR) / sizeof(ExpressionTypeSTR[0]) == static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length));

	class ScopedSymbolTable;

	static inline bool ExpressionTypeIsBuiltIn(ExpressionType type) {
		switch (type) {
		case ExpressionType::BOOLEAN:
		case ExpressionType::FLOAT:
		case ExpressionType::INT:
		case ExpressionType::STRING:
			return true;
		default:
			return false;
		}
	}

	static std::unordered_map<std::string, ExpressionType> ExpressionTypeMapBuild() {
    	auto result = std::unordered_map<std::string, ExpressionType>{};
    	static constexpr auto ExpressionTypeSize = static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length);
    	for(auto index = 0u; index < ExpressionTypeSize; index++) {
        	result[ExpressionTypeSTR[index]] = static_cast<ExpressionType>(index);
    	}
    	return result;
	}

	static const auto ExpressionTypeMap = ExpressionTypeMapBuild();

}

