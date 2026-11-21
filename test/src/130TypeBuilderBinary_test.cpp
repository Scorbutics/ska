#include <iostream>

#include <doctest.h>
#include "TypeBuilderTestCommon.h"
#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

#include "Service/TypeBuilder/TypeBuilderBinary.h"
#include "Service/TypeBuilder/TypeBuilderLiteral.h"
#include "NodeValue/ScriptHandleAST.h"

TEST_CASE("[TypeBuilderBinary]") {
	auto scriptCache = ska::ScriptCacheAST{};
	DataTestContainer data;
	auto script = TypeBuilderTestCommonBuildAST(scriptCache, "", data, false);
	script.parse(*data.parser);

	auto typeCrosser = ska::TypeCrosser{};

	auto typeBuilder = ska::TypeBuilderOperator<ska::Operator::BINARY>{ typeCrosser };

	auto value1Node = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::STRING, {}});
	auto value2Node = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::STRING, {}});
	auto node = ska::ASTFactory::MakeLogicalNode(ska::Token{"+", ska::TokenType::SYMBOL, {}}, std::move(value1Node), std::move(value2Node));

	auto result = std::array<ska::TypeBuildUnitPtr, static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length)>{};
	result[static_cast<std::size_t>(ska::Operator::BINARY)] = std::make_unique<ska::TypeBuilderOperator<ska::Operator::BINARY>>(typeCrosser);
	result[static_cast<std::size_t>(ska::Operator::UNARY)] = std::make_unique<ska::TypeBuilderOperator<ska::Operator::UNARY>>();
	result[static_cast<std::size_t>(ska::Operator::LITERAL)] = std::make_unique<ska::TypeBuilderOperator<ska::Operator::LITERAL>>();

	for(auto& child : *node) {
		child->updateType(result[static_cast<std::size_t>(child->op())]->build(script, *child).type);
	}

	//This one makes a "crossType"
	auto type = typeBuilder.build(script, *node).type;
	CHECK(type == ska::ExpressionType::STRING);

	// No implicit conversion : int + string is not a type
	auto intNode = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::DIGIT, {}});
	auto stringNode = ska::ASTFactory::MakeLogicalNode(ska::Token{"1", ska::TokenType::STRING, {}});
	auto mixedNode = ska::ASTFactory::MakeLogicalNode(ska::Token{"+", ska::TokenType::SYMBOL, {}}, std::move(intNode), std::move(stringNode));
	for(auto& child : *mixedNode) {
		child->updateType(result[static_cast<std::size_t>(child->op())]->build(script, *child).type);
	}
	CHECK_THROWS(typeBuilder.build(script, *mixedNode));
}
