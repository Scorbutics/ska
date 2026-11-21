#include <algorithm>
#include <doctest.h>
#include "Config/LoggerConfigLang.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"

TEST_CASE("test") {
	const auto inputStr = std::string("for(i = 0\n i < 5\n i++) do lol(\"mdr\", 12) end");
	const auto keywords = ska::ReservedKeywordsPool{};
	auto t = ska::Tokenizer {keywords, inputStr};
	auto tokens = t.tokenize();

	CHECK(tokens.size() == 21);
	auto index = 0u;
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::FOR>());
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
	CHECK(tokens[index++] == ska::Token{"i", ska::TokenType::IDENTIFIER, {}});
	CHECK(tokens[index++] == ska::Token{"=", ska::TokenType::SYMBOL, {}});
	CHECK(tokens[index++] == ska::Token{"0", ska::TokenType::DIGIT, {}});
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	CHECK(tokens[index++] == ska::Token{"i", ska::TokenType::IDENTIFIER, {}});
	CHECK(tokens[index++] == ska::Token{"<", ska::TokenType::SYMBOL, {}});
	CHECK(tokens[index++] == ska::Token{"5", ska::TokenType::DIGIT, {}});
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	CHECK(tokens[index++] == ska::Token{"i", ska::TokenType::IDENTIFIER, {}});
	CHECK(tokens[index++] == ska::Token{"++", ska::TokenType::SYMBOL, {}});
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::BLOCK_BEGIN>());
	CHECK(tokens[index++] == ska::Token { "lol", ska::TokenType::IDENTIFIER, {}});
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
	CHECK(tokens[index++] == ska::Token { "mdr", ska::TokenType::STRING, {}});
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::ARGUMENT_DELIMITER>());
	CHECK(tokens[index++] == ska::Token { "12", ska::TokenType::DIGIT, {}});
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	CHECK(tokens[index++] == keywords.pattern<ska::TokenGrammar::BLOCK_END>());
}

TEST_CASE("Symbol by symbol") {
	const auto keywords = ska::ReservedKeywordsPool{};
	
	SUBCASE("for") {
		const auto input = std::string("for");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FOR>());
	}

	SUBCASE("var") {
		const auto input = std::string("var");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::VARIABLE>());
	}

	SUBCASE("int") {
		const auto input = std::string("int");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::INT>());
	}
	
	SUBCASE("float") {
		const auto input = std::string("float");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FLOAT>());
	}
	
	SUBCASE("string") {
		const auto input = std::string("string");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::STRING>());
	}
	
	SUBCASE("if") {
		const auto input = std::string("if");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::IF>());
	}

	SUBCASE("else") {
		const auto input = std::string("else");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::ELSE>());
	}

	SUBCASE("return") {
		const auto input = std::string("return");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::RETURN>());
	}

	SUBCASE("function") {
		const auto input = std::string("function");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FUNCTION>());
	}

	SUBCASE("any identifier") {
		const auto input = std::string("testId");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token{"testId", ska::TokenType::IDENTIFIER, {}});
	}

	SUBCASE("end statement symbol") {
		const auto input = std::string("\n");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	}

	SUBCASE("block begin symbol") {
		const auto input = std::string("do");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::BLOCK_BEGIN>());
	}

	SUBCASE("block end symbol") {
		const auto input = std::string("end");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::BLOCK_END>());
	}

	SUBCASE("object block begin symbol") {
		const auto input = std::string("{");
		auto t = ska::Tokenizer{ keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::OBJECT_BLOCK_BEGIN>());
	}

	SUBCASE("object block end symbol") {
		const auto input = std::string("}");
		auto t = ska::Tokenizer{ keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::OBJECT_BLOCK_END>());
	}

	SUBCASE("filter symbol") {
		const auto input = std::string("|");
		auto t = ska::Tokenizer{ keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FILTER>());
	}

	SUBCASE("parenthesis begin") {
		const auto input = std::string("(");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
	}

	SUBCASE("parenthesis end") {
		const auto input = std::string(")");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	}

	SUBCASE("random symbol : equal sign affectation") {
		const auto input = std::string("=");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"=", ska::TokenType::SYMBOL, {}});
	}

	SUBCASE("integer") {
		const auto input = std::string("234");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token { "234", ska::TokenType::DIGIT, {}});
	}
	
	SUBCASE("real number") {
		const auto input = std::string("32.1");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"32.1", ska::TokenType::DIGIT, {}});
	}

	SUBCASE("Dot symbol (for field access)") {
		const auto input = std::string(".");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::FIELD_ACCESS_OPERATOR>());
	}

	SUBCASE("End statement") {
		const auto input = std::string("\n");
		auto t = ska::Tokenizer{ keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	}

	SUBCASE("Several end statements") {
		const auto input = std::string("\n\n\n \n \n\n \n\n");
		auto t = ska::Tokenizer{ keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == keywords.pattern<ska::TokenGrammar::STATEMENT_END>());
	}

	SUBCASE("string") {
		const auto input = std::string("\"tettttt\"");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"tettttt", ska::TokenType::STRING, {}});
	}

	SUBCASE("Full method call 0 args") {
		const auto input = std::string("test.call()");
		auto t = ska::Tokenizer {keywords, input};
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 5);
		CHECK(tokens[0] == ska::Token { "test", ska::TokenType::IDENTIFIER, {}});
		CHECK(tokens[1] == keywords.pattern<ska::TokenGrammar::FIELD_ACCESS_OPERATOR>());
		CHECK(tokens[2] == ska::Token { "call", ska::TokenType::IDENTIFIER, {}});
		CHECK(tokens[3] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_BEGIN>());
		CHECK(tokens[4] == keywords.pattern<ska::TokenGrammar::PARENTHESIS_END>());
	}

	SUBCASE("only spaces") {
		const auto input = std::string("  \t ");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.empty());
	}

	SUBCASE("spaces with not space in middle") {
		const auto input = std::string("  \t o  ");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token { "o", ska::TokenType::IDENTIFIER, {}});
	}
	
	SUBCASE("string with noise around") {
		const auto input = std::string(" 13.23  \"testString\" var");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 3);
		CHECK(tokens[0] == ska::Token { "13.23", ska::TokenType::DIGIT , {}});
		CHECK(tokens[1] == ska::Token { "testString", ska::TokenType::STRING, {}});
		CHECK(tokens[2] == keywords.pattern<ska::TokenGrammar::VARIABLE>());
	}

	SUBCASE("string with reserved symbols and keywords") {
		const auto input = std::string("\"var i = 0; {}\"");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token { "var i = 0; {}", ska::TokenType::STRING, {}});
	}

	SUBCASE("symbols in queue") {
		const auto input = std::string(";;++--");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();
		
		CHECK(tokens.size() == 4);
		CHECK(tokens[0] == ska::Token {";", ska::TokenType::SYMBOL, {}});
		CHECK(tokens[1] == ska::Token {";", ska::TokenType::SYMBOL, {}});
		CHECK(tokens[2] == ska::Token {"++", ska::TokenType::SYMBOL, {}});
		CHECK(tokens[3] == ska::Token {"--", ska::TokenType::SYMBOL, {}});
	}

	SUBCASE("double symbols tokens") {
		const auto input = std::string("--");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token {"--", ska::TokenType::SYMBOL, {}} );
	}

	SUBCASE("lot of spaces and whitespaces characters") {
		const auto input = std::string("       \t     ");
		auto t = ska::Tokenizer { keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.empty());
	}

	SUBCASE("string litteral with spaces at end : not trimed") {
		const auto input = std::string("\"test   \"");
		auto t = ska::Tokenizer{ keywords, input };
		auto tokens = t.tokenize();

		CHECK(tokens.size() == 1);
		CHECK(tokens[0] == ska::Token{ "test   ", ska::TokenType::STRING, {} });
	}
	
	SUBCASE("unterminated string") {
		const auto input = std::string(" \"   1234 eetr ;");
		auto t = ska::Tokenizer { keywords, input };
		
		try {
			auto tokens = t.tokenize();
			CHECK(false);
		} catch (std::exception& e) {
			CHECK(std::string{e.what()}.find("unterminated token") != std::string::npos);
		}
	}
}


TEST_CASE("Comments : from a '#' to the end of the line, ignored") {
	const auto keywords = ska::ReservedKeywordsPool{};
	const auto withComments = ska::Tokenizer {keywords, "# a whole line comment\na = 1 # a trailing comment\n#\nb = \"# not a comment\" # but this is\n"}.tokenize();
	const auto withoutComments = ska::Tokenizer {keywords, "\na = 1\n\nb = \"# not a comment\"\n"}.tokenize();
	REQUIRE(withComments.size() == withoutComments.size());
	for (auto index = 0u; index < withComments.size(); index++) {
		CHECK(withComments[index] == withoutComments[index]);
	}
	auto stringToken = std::find_if(withComments.begin(), withComments.end(), [](const ska::Token& token) { return token.type() == ska::TokenType::STRING; });
	REQUIRE(stringToken != withComments.end());
	CHECK(*stringToken == ska::Token { "# not a comment", ska::TokenType::STRING, {} });
}
