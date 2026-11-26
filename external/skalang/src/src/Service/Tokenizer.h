#pragma once
#include <string>
#include <vector>
#include <bitset>
#include <cctype>
#include <unordered_set>

#include "ReservedKeywordsPool.h"
#include "NodeValue/Token.h"

namespace ska {

	struct RequiredToken {
		ska::TokenType current = ska::TokenType::EMPTY;
		bool required = true;
		std::bitset<static_cast<std::size_t>(TokenType::UNUSED_LAST_Length)> requiredOrUntil;
	};

	enum class SymbolFinalizeTokenState {
		READY,
		FINALIZED_WITH_NEXT_SYMBOL,
		FINALIZED_NO_NEXT_SYMBOL
	};

	class Tokenizer {
	public:
		Tokenizer(const ReservedKeywordsPool& reserved, std::string input);
		std::vector<Token> tokenize() const;

	private:
		static std::unordered_set<std::string> BuildAllowedMultipleCharTokenSymbolsSet();

		std::string getInputStringTokenOrThrow(const TokenType& tokenType, std::size_t index, const Cursor& lastCursor, const std::size_t offset) const;
		RequiredToken determineCurrentToken(const std::size_t startIndex) const;
		std::pair<Cursor, Token> tokenizeNext(const RequiredToken& requiredToken, const Cursor& lastCursor) const;
		RequiredToken initializeCharType(const TokenType charTokenType) const;
		std::string finalizeRawToken(std::size_t index, const RequiredToken& requiredToken, const Cursor& lastCursor) const;
		std::string postComputing(std::size_t index, const RequiredToken& requiredToken, const Cursor& lastCursor) const;
		TokenType calculateCharacterTokenType(const char c) const;
		std::pair<ska::Token, ska::Token> stackToken(TokenType currentType, Token token, std::vector<Token>& stackSymbol) const;

		static bool isWordCharacter(const int c) {
			return std::isalnum(c) || c == '_';
		}
		
		static bool isNotBlankToken(TokenType token) {
			return token != ska::TokenType::EMPTY &&
				token != ska::TokenType::SPACE;
		}

		Cursor computeTokenPositionCursor(std::size_t index, const std::string& readToken, TokenGrammar grammar, bool wasRequired, const Cursor& lastCursor) const;

		Token group(std::vector<Token>& symbolStack) const;
		void push(Token t, std::vector<Token>& output) const;

		static SymbolFinalizeTokenState isFinalizedSymbol(char nextSymbol, const std::vector<Token>& symbolStack);

		static int stopSymbolCharAggregation(char symbol);
		
		static const std::unordered_set<std::string> ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS;
		const ReservedKeywordsPool& m_reserved;
		std::string m_input;
	};
}
