#include "Config/LoggerConfigLang.h"
#include "MatcherType.h"

#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/Symbol.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherType)

ska::ASTNodePtr ska::MatcherType::match(TokenReader& input, std::string* typeStr) {
	const auto& typeDelimiterToken = m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>();

	bool isBuiltIn = true;
	auto nodes = std::vector<ASTNodePtr> {};
	if (input.expect(TokenType::IDENTIFIER)) {
		auto typeNamespaceToken = input.match(TokenType::IDENTIFIER);

		if (typeStr != nullptr) {
			*typeStr += typeNamespaceToken.name();
		}

		//Handles script namespace
		auto complexTypeToken = Token{};
		if (input.expect(typeDelimiterToken)) {
			input.match(typeDelimiterToken);
			input.match(typeDelimiterToken);
			complexTypeToken = input.match(TokenType::IDENTIFIER);

			if (typeStr != nullptr) {
				*typeStr += typeDelimiterToken.name() + typeDelimiterToken.name() + complexTypeToken.name();
			}
		}
		nodes.push_back(ASTFactory::MakeLogicalNode(std::move(typeNamespaceToken), complexTypeToken.empty() ? nullptr : ASTFactory::MakeLogicalNode(std::move(complexTypeToken))));
		isBuiltIn = false;
	} else {
		auto reservedTokenType = input.match(TokenType::RESERVED);
		if (typeStr != nullptr) {
			*typeStr += reservedTokenType.name();
		}
		nodes.push_back(ASTFactory::MakeLogicalNode(std::move(reservedTokenType)));
	}

	//handle objects
	if (input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>())) {
		const auto firstParenthesis = input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

		if (isBuiltIn) {
			throw std::runtime_error("syntax error : \"" + nodes[0]->name() + "\" is used as a function-object but is a built-in type");
		}

		if (typeStr != nullptr) {
			*typeStr += m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>().name() + m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>().name();
		}

		nodes.push_back(ASTFactory::MakeLogicalNode(Token{ "", TokenType::RANGE, firstParenthesis.position() }));
	} else {
		nodes.push_back(ASTFactory::MakeEmptyNode());
	}

	//handle arrays (T[]) and maps (T[:])
	if (input.expect(TokenType::ARRAY)) {
		const auto firstBracket = input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>());
		const auto isMap = input.expect(typeDelimiterToken);
		if (isMap) {
			input.match(typeDelimiterToken);
		}
		input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>());

		if (typeStr != nullptr) {
			*typeStr += m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_BEGIN>().name() + (isMap ? typeDelimiterToken.name() : "") + m_reservedKeywordsPool.pattern<TokenGrammar::BRACKET_END>().name();
		}

		nodes.push_back(ASTFactory::MakeLogicalNode(Token{ isMap ? ":" : "", TokenType::ARRAY, firstBracket.position()}));
	} else {
		nodes.push_back(ASTFactory::MakeEmptyNode());
	}

	assert(nodes.size() == 3);
	return ASTFactory::MakeNode<Operator::TYPE>(std::move(nodes));
}

void ska::MatcherType::malformedType(const Type& input, const std::string& additionalMessage) {
	auto ss = std::stringstream {};
	ss << "malformed type : " << input << additionalMessage;
	throw std::runtime_error(ss.str());
}

ska::ASTNodePtr ska::MatcherType::match(const Type& input) {
	auto nodes = std::vector<ASTNodePtr> {};
	if(Type::isBuiltIn(input)) {
		SLOG(LogLevel::Info) << "Type " << input << " is built-in";
		const auto index = static_cast<std::size_t>(input.type());
		SLOG(LogLevel::Debug) << " Index " << index;
		auto typeStr = ExpressionTypeSTR[index];
		auto tokenInfo = m_reservedKeywordsPool.pool.at(typeStr);
		nodes.push_back(ASTFactory::MakeLogicalNode(tokenInfo.token));
		nodes.push_back(ASTFactory::MakeEmptyNode());
		nodes.push_back(ASTFactory::MakeEmptyNode());
		SLOG(LogLevel::Info) << " Has token " << tokenInfo.token;
	} else {
		// ARRAY, VAR, FUNCTION (not built-ins !)

		SLOG(LogLevel::Info) << "Type " << input << " is not built-in";
		if(input.size() > 2) {
			malformedType(input);
		}

		switch(input.type()) {
			case ExpressionType::ARRAY:
				if(input.size() != 1) {
					malformedType(input, " : bad type size in array type (should be 1)");
				}
				nodes.push_back(match(input[0]));
				nodes.push_back(ASTFactory::MakeEmptyNode());
				nodes.push_back(ASTFactory::MakeLogicalNode(Token{ "", TokenType::ARRAY, {}}));
				break;
			case ExpressionType::MAP:
				if(input.size() != 1) {
					malformedType(input, " : bad type size in map type (should be 1)");
				}
				nodes.push_back(match(input[0]));
				nodes.push_back(ASTFactory::MakeEmptyNode());
				nodes.push_back(ASTFactory::MakeLogicalNode(Token{ ":", TokenType::ARRAY, {}}));
				break;
			case ExpressionType::FUNCTION:
			case ExpressionType::OBJECT: {
				auto symbol = input.name();
				SLOG(LogLevel::Info) << "Type " << input << " is an object/function named " << symbol;
				auto token = symbol.empty() ? m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>() : Token{ symbol, TokenType::IDENTIFIER, {} };
				nodes.push_back(ASTFactory::MakeLogicalNode(std::move(token)));
				nodes.push_back(input.type() == ExpressionType::OBJECT ? ASTFactory::MakeLogicalNode(Token { "", TokenType::RANGE, {}}) : ASTFactory::MakeEmptyNode());
				nodes.push_back(ASTFactory::MakeEmptyNode());
				} break;
			default:
				malformedType(input);
				break;
		}
	}
	assert(nodes.size() == 3);
	return ASTFactory::MakeNode<Operator::TYPE>(std::move(nodes));
}
