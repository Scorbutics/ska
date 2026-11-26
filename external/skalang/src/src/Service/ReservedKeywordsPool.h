#pragma once

#include <unordered_map>
#include <vector>
#include "NodeValue/Token.h"

namespace ska {

	struct ReservedKeywordsPool {
		struct TokenInfo {
			std::string patternString;
			Token token;
		};

		ReservedKeywordsPool() :
			pool(BuildPool()),
			patterns(BuildPatterns(pool)){
		}

		const std::unordered_map<std::string, TokenInfo> pool {};

		TokenGrammar grammar(const std::string& tokenValueStr) const;

		template <TokenGrammar token>
		const Token& pattern() const {
			return patterns[static_cast<std::size_t>(token)].token;
		}

		const Token& pattern(TokenGrammar token) const {
			return patterns[static_cast<std::size_t>(token)].token;
		}

    	const std::string& patternString(const std::size_t token) const {
			return patterns[token].patternString;
		}

	private:
		const std::vector<TokenInfo> patterns;

		static std::vector<TokenInfo> BuildPatterns(const std::unordered_map<std::string, TokenInfo>& pool);
		static std::unordered_map<std::string, TokenInfo> BuildPool();
	};
}
