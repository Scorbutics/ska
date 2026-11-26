#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "NodeValue/ExpressionType.h"
#include "Service/Tokenizer.h"
#include "Service/SymbolTable.h"
#include "Service/StatementParser.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/ScriptHandleAST.h"

using SymbolTablePtr = std::unique_ptr<ska::SymbolTable>;
using ParserPtr = std::unique_ptr<ska::StatementParser>;

static const auto reservedKeywords = ska::ReservedKeywordsPool{};

ska::ScriptAST ASTFromInputSemanticExpressionType(ska::ScriptCacheAST& scriptCache, const std::string& input, ParserPtr& parser_test) {
	static auto refCounter = 0;

	auto tokenizer = ska::Tokenizer { reservedKeywords, input };
	const auto tokens = tokenizer.tokenize();
	auto reader = ska::ScriptAST { scriptCache, "main", std::move(tokens) };
	parser_test = std::make_unique<ska::StatementParser> ( reservedKeywords );
	return reader;
}

TEST_CASE("[ExpressionType]") {
	ParserPtr parser_test;
	auto scriptCache = ska::ScriptCacheAST{};
	auto script = ASTFromInputSemanticExpressionType(scriptCache, "do toto = 2\n end", parser_test);
	script.parse(*parser_test);
	auto* symbol_test = &script.symbols();

	SUBCASE("Type is set") {
		auto type = ska::Type::MakeCustom<ska::ExpressionType::FUNCTION>((*symbol_test)["toto"]);
	CHECK(type == ska::ExpressionType::FUNCTION);

		auto type2 = ska::Type::MakeBuiltIn<ska::ExpressionType::FLOAT>();
		CHECK(type2 == ska::ExpressionType::FLOAT);
	}

	SUBCASE("Type Copy") {
		auto type = ska::Type::MakeCustom<ska::ExpressionType::OBJECT>((*symbol_test)["toto"]);
		type.add(ska::Type::MakeBuiltIn<ska::ExpressionType::INT>());
	auto typeCopied = type;
	CHECK(typeCopied == ska::ExpressionType::OBJECT);
	CHECK(!typeCopied.empty());
	CHECK(typeCopied[0] == ska::ExpressionType::INT);
	CHECK(typeCopied == type);
	}

	SUBCASE("Type Move") {
		auto type = ska::Type::MakeCustom<ska::ExpressionType::OBJECT>((*symbol_test)["toto"]);
		type.add(ska::Type::MakeBuiltIn<ska::ExpressionType::INT>());
	auto typeMoved = std::move(type);
	CHECK(typeMoved == ska::ExpressionType::OBJECT);
	CHECK(!typeMoved.empty());
	CHECK(typeMoved[0] == ska::ExpressionType::INT);
	}
}
