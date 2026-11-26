#pragma once
#include <unordered_map>

namespace ska {
	enum class LogicalOperator {
    	ADDITION,
    	MULTIPLY,
    	DIVIDE,
    	MODULO,
    	SUBSTRACT,
    	EQUAL,
		EQUALITY,
    	CONDITION,
		INEQUALITY,
    	NOT,
    	ADDITION_EQUAL,
    	SUBSTRACT_EQUAL,
    	MULTIPLY_EQUAL,
    	DIVIDE_EQUAL,
		LESSER,
		LESSER_OR_EQUAL,
		GREATER_OR_EQUAL,
		GREATER,
    	AND,
    	OR,
    	UNUSED_Last_Length
	};

	static const auto LogicalOperatorMap = std::unordered_map<std::string, LogicalOperator>{
    	{"+",  LogicalOperator::ADDITION},
    	{"*",  LogicalOperator::MULTIPLY},
    	{"/",  LogicalOperator::DIVIDE},
    	{"%",  LogicalOperator::MODULO},
    	{"-",  LogicalOperator::SUBSTRACT},
    	{"=",  LogicalOperator::EQUAL},
    	{"==", LogicalOperator::EQUALITY},
		{"<=", LogicalOperator::LESSER_OR_EQUAL},
		{">=", LogicalOperator::GREATER_OR_EQUAL},
		{"<",  LogicalOperator::LESSER},
		{">",  LogicalOperator::GREATER},
    	{"!=", LogicalOperator::INEQUALITY},
    	{"!",  LogicalOperator::NOT},
    	{"+=", LogicalOperator::ADDITION_EQUAL},
    	{"-=", LogicalOperator::SUBSTRACT_EQUAL},
    	{"*=", LogicalOperator::MULTIPLY_EQUAL},
    	{"/=", LogicalOperator::DIVIDE_EQUAL},
    	{"&&", LogicalOperator::AND},
    	{"||", LogicalOperator::OR}
	};
}
