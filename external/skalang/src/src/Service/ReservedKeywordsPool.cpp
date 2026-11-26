#include <vector>
#include "Config/LoggerConfigLang.h"
#include "ReservedKeywordsPool.h"

std::unordered_map<std::string, ska::ReservedKeywordsPool::TokenInfo> ska::ReservedKeywordsPool::BuildPool() {
	auto pool = std::unordered_map<std::string, TokenInfo>{};
	
	const auto emplacer = [&](TokenGrammar grammar, TokenType type) {
		const auto index = static_cast<std::size_t>(grammar);
		if(type == TokenType::RESERVED) {
			pool.emplace(TokenGrammarSTR[index], TokenInfo { TokenGrammarSTR[index], Token{ index, TokenType::RESERVED, {}, grammar } });
		} else {
			pool.emplace(TokenGrammarSTR[index], TokenInfo { TokenGrammarSTR[index], Token{ TokenGrammarSTR[index], std::move(type), {}, grammar } });
		}
	};

	for(auto index = 0u; index < static_cast<std::size_t>(TokenGrammar::FIELD_ACCESS_OPERATOR); index++) {
		emplacer(static_cast<TokenGrammar>(index), TokenType::RESERVED);
	}

	emplacer(TokenGrammar::FIELD_ACCESS_OPERATOR, TokenType::DOT_SYMBOL);
	emplacer(TokenGrammar::AFFECTATION, TokenType::SYMBOL);
	emplacer(TokenGrammar::BLOCK_BEGIN, TokenType::RANGE);
	emplacer(TokenGrammar::BLOCK_END, TokenType::RANGE);
	emplacer(TokenGrammar::OBJECT_BLOCK_BEGIN, TokenType::RANGE);
	emplacer(TokenGrammar::OBJECT_BLOCK_END, TokenType::RANGE);
	emplacer(TokenGrammar::PARENTHESIS_BEGIN, TokenType::RANGE);
	emplacer(TokenGrammar::PARENTHESIS_END, TokenType::RANGE);
	emplacer(TokenGrammar::BRACKET_BEGIN, TokenType::ARRAY);
	emplacer(TokenGrammar::BRACKET_END, TokenType::ARRAY);
	emplacer(TokenGrammar::STRING_DELIMITER, TokenType::SYMBOL);
	emplacer(TokenGrammar::STATEMENT_END, TokenType::END_STATEMENT);
	emplacer(TokenGrammar::ARGUMENT_DELIMITER, TokenType::SYMBOL);
	emplacer(TokenGrammar::TYPE_DELIMITER, TokenType::SYMBOL);
	emplacer(TokenGrammar::FILTER, TokenType::SYMBOL);
	emplacer(TokenGrammar::TRUE, TokenType::BOOLEAN);
	emplacer(TokenGrammar::FALSE, TokenType::BOOLEAN);
	return pool;
	
}

std::vector<ska::ReservedKeywordsPool::TokenInfo> ska::ReservedKeywordsPool::BuildPatterns(const std::unordered_map<std::string, TokenInfo>& pool) {
	auto patterns = std::vector<TokenInfo>();
	patterns.resize(pool.size());

	auto index = 0u;
	for (const auto& p : pool) {
		if(p.second.token.type() == TokenType::RESERVED) {
			patterns[std::get<std::size_t>(p.second.token.content())] = p.second;
		}
	}

	const auto enumLength = static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length);
	for(auto index = static_cast<std::size_t>(TokenGrammar::FIELD_ACCESS_OPERATOR); index < enumLength; index++) {
		patterns[index] = pool.at(TokenGrammarSTR[index]);
	}

	return patterns;
}

ska::TokenGrammar ska::ReservedKeywordsPool::grammar(const std::string& tokenValueStr) const{
	const auto inReservedPoolIt = pool.find(tokenValueStr);
	if (inReservedPoolIt != pool.end()) {
		return inReservedPoolIt->second.token.grammar();
	}
	return TokenGrammar::UNUSED_Last_Length;
}
