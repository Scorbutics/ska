#pragma once

namespace ska {
	enum class TokenGrammar {
		//Reserved keywords
		FOR,
		IF,
		ELSE,
		VARIABLE,
		FUNCTION,
		RETURN,
		IMPORT,
		BOOLEAN,
    	INT,
		FLOAT,
		STRING,
		VOID,
		WHILE,
		ENUM,
		MATCH,
		WHEN,
		THEN,
		//Reserved symbols
		FIELD_ACCESS_OPERATOR,
		AFFECTATION,
		BLOCK_BEGIN,
		BLOCK_END,
		OBJECT_BLOCK_BEGIN,
		OBJECT_BLOCK_END,
		PARENTHESIS_BEGIN,
		PARENTHESIS_END,
		BRACKET_BEGIN,
		BRACKET_END,
		STRING_DELIMITER,
		STATEMENT_END,
		ARGUMENT_DELIMITER,
		TYPE_DELIMITER,
		FILTER,
		//Reserved values
		TRUE,
		FALSE,
		//The last one for enum length
		UNUSED_Last_Length
	};


	constexpr const char* TokenGrammarSTR[] = {
		"for",
		"if",
		"else",
		"var",
		"function",
		"return",
		"import",
    	"bool",
		"int",
		"float",
		"string",
		"void",
		"while",
		"enum",
		"match",
		"when",
		"then",
		".",
		"=",
		"do",
		"end",
		"{",
		"}",
		"(",
		")",
		"[",
		"]",
		"\"",
		"\n",
		",",
		":",
		"|",
		"true",
		"false"
	};

	static_assert(sizeof(TokenGrammarSTR) / sizeof(TokenGrammarSTR[0]) == static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length));
}
