#include <iterator>
#include <fstream>
#include "Matcher/MatcherBlock.h"
#include "Config/LoggerConfigLang.h"
#include "Service/LanguageHints.h"
#include "NodeValue/AST.h"
#include "NodeValue/Operator.h"
#include "Service/StatementParser.h"
#include "ReservedKeywordsPool.h"

#include "Service/Tokenizer.h"
#include "Service/TokenReader.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

#include "Error/LangError.h"
#include "Error/ParserError.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::StatementParser)

ska::StatementParser::StatementParser(const ReservedKeywordsPool& reservedKeywordsPool) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_expressionParser(reservedKeywordsPool, *this),
	m_matcherBlock(m_reservedKeywordsPool, *this),
	m_matcherFor(m_reservedKeywordsPool, *this),
	m_matcherIfElse(m_reservedKeywordsPool, *this, m_matcherBlock),
	m_matcherVar(m_reservedKeywordsPool, *this),
	m_matcherType(m_reservedKeywordsPool),
	m_matcherConverter(m_reservedKeywordsPool, *this, m_matcherType),
	m_matcherReturn(m_reservedKeywordsPool, *this, m_matcherConverter),
	m_matcherImport(m_reservedKeywordsPool, *this),
	m_matcherFilter(m_reservedKeywordsPool, *this, m_matcherBlock) {
}

ska::StatementParser::ASTNodePtr ska::StatementParser::parse(ScriptAST& input) {
	if (input.reader().empty()) {
		return nullptr;
	}

	try {
		auto blockNodeStatements = std::vector<ASTNodePtr>{};
			while (!input.reader().empty()) {
			auto optionalStatement = optstatement(input);
			if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
				blockNodeStatements.push_back(std::move(optionalStatement));
			}
		}
		auto scriptNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::SCRIPT_OBJECT>(std::move(blockNodeStatements)));
		auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*scriptNode, input);
		observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
		SLOG(ska::LogLevel::Debug) << "Script built: " << *scriptNode;
		return scriptNode;
	} catch (std::exception& e) {
		throw LangError(input.name(), e);
	}
}

ska::StatementParser::ASTNodePtr ska::StatementParser::statement(ScriptAST& input) {
	if (input.reader().empty()) {
		return nullptr;
	}

	const auto token = input.reader().actual();
	try {
		switch (token.type()) {
		case TokenType::RESERVED:
			return matchReservedKeyword(input, std::get<std::size_t>(token.content()));

		case TokenType::RANGE:
			return m_matcherBlock.match(input);

		default:
			return matchExpressionStatement(input);
		}
	} catch (ParserError& error) {
		throw error;
	} catch (std::runtime_error& error) {
		throw ParserError(error.what(), input.reader().actual().position());
	}
}

ska::StatementParser::ASTNodePtr ska::StatementParser::matchExpressionStatement(ScriptAST& input) {
	SLOG(ska::LogLevel::Info) << "Expression-statement found";

	auto expressionResult = expr(input);

	if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::FILTER>())) {
		return m_matcherFilter.match(input, std::move(expressionResult));
	}

	if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>())) {
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
	} else if (!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
		error("expected block-end statement");
	}

	if (expressionResult == nullptr) {
			SLOG(ska::LogLevel::Info) << "NOP statement";
			return nullptr;
	}
	return expressionResult;
}

ska::StatementParser::ASTNodePtr ska::StatementParser::matchReservedKeyword(ScriptAST& input, const std::size_t keywordIndex) {
	switch (keywordIndex) {
	case static_cast<std::size_t>(TokenGrammar::FOR):
		return m_matcherFor.match(input);

	case static_cast<std::size_t>(TokenGrammar::WHILE):
		return m_matcherFor.matchWhile(input);

	case static_cast<std::size_t>(TokenGrammar::IF):
		return m_matcherIfElse.match(input);

	case static_cast<std::size_t>(TokenGrammar::VARIABLE):
		// "var" is a type (objects) : it does not start a statement
		error(hints::VariableKeyword());
		return nullptr;

	case static_cast<std::size_t>(TokenGrammar::FUNCTION):
		return expr(input);

	case static_cast<std::size_t>(TokenGrammar::RETURN):
		return m_matcherReturn.match(input);

	default: {
			std::stringstream ss;
			ss << (keywordIndex < static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length) ? TokenGrammarSTR[keywordIndex] : "UNKNOWN TYPE" );
			error("Unhandled keyword type : " + ss.str());
			return nullptr;
		}
	}
}

ska::StatementParser::ASTNodePtr ska::StatementParser::expr(ScriptAST& input) {
	return m_expressionParser.parse(input);
}

ska::StatementParser::ASTNodePtr ska::StatementParser::exprMapKey(ScriptAST& input) {
	return m_expressionParser.parseMapKey(input);
}

void ska::StatementParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}

ska::StatementParser::ASTNodePtr ska::StatementParser::optexpr(ScriptAST& input, const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!input.reader().expect(mustNotBe)) {
		node = expr(input);
	}
	return node != nullptr ? std::move(node) : ASTFactory::MakeEmptyNode();
}

ska::ScriptASTPtr ska::StatementParser::subParse(ScriptCacheAST& scriptCache, const std::string& name, std::ifstream& file) {
	auto content = std::string (
		(std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>())
	);

	const auto scriptAlreadyExists = scriptCache.find(name) != scriptCache.end();
	SLOG(ska::LogLevel::Info) << "SubParsing script " << name << " : " << (scriptAlreadyExists ? "not " : "") << "in cache";
	if (scriptAlreadyExists) {
		auto existing = std::make_unique<ScriptAST>(scriptCache.at(name));
		if (existing->failed()) {
			throw std::runtime_error("the script \"" + name + "\" could not be compiled");
		}
		return existing;
	}

	// The last statement of a script file ends with the end of the file, even without a final line break
	if (!content.empty() && content.back() != '\n') {
		content.push_back('\n');
	}
	auto tokenizer = Tokenizer{ m_reservedKeywordsPool, std::move(content) };
	auto tokens = tokenizer.tokenize();
	auto script = ScriptASTPtr{std::make_unique<ScriptAST>( scriptCache, name, tokens )};
	try {
		script->parse(*this);
	} catch (...) {
		// The script stays in the cache (it has an index) : it is marked as failed, not used again
		script->setFailed();
		throw;
	}
	return script;
}

ska::StatementParser::ASTNodePtr ska::StatementParser::optstatement(ScriptAST& input, const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!input.reader().expect(mustNotBe)) {
		node = statement(input);
	}

	return node != nullptr ? std::move(node) : ASTFactory::MakeEmptyNode();
}

