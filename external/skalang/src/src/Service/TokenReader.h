#pragma once
#include <vector>
#include <functional>
#include <unordered_set>
#include "NodeValue/Token.h"

namespace ska {
	using TokenReaderExpectCallback = std::function<bool(const Token&)>;

	struct TokenReaderExpect {
		static TokenReaderExpectCallback value(const Token::Variant& expectedValue){
			return [&expectedValue] (const Token & value) {
				return expectedValue == value.content();
			};
		}

		static TokenReaderExpectCallback type(const TokenType& expectedValue) {
			return [&expectedValue](const Token& value) {
				return expectedValue == value.type();
			};
		}
	};

	class TokenReader {
	public:
    	TokenReader(std::vector<ska::Token> input, std::size_t startIndex = 0) :
        	m_input(std::move(input)),
        	m_lookAheadIndex{ startIndex } {
			if (!m_input.empty()) {
				m_lookAhead = &m_input[m_lookAheadIndex];
			}
    	}

		TokenReader(const TokenReader&) = delete;
		TokenReader(TokenReader&&) noexcept = default;
		TokenReader& operator=(const TokenReader&) = delete;
		TokenReader& operator=(TokenReader&&) = default;
		~TokenReader() = default;

		bool emptyTokens() const;
		const Token* mightMatch(const Token& token);
		const Token& match(const Token& t);
		const Token& match(const TokenType type);
		bool expect(const Token& token) const;
		bool expect(const TokenType& type) const;
		bool empty() const;
		Token actual() const;
		bool expectOneType(const std::unordered_set<TokenGrammar>& inList) const;
		const Token& readPrevious(std::size_t offset) const;
		bool canReadPrevious(std::size_t offset) const;
		void rewind();

	private:
		void checkNotEof() const;
    	void nextToken();
		[[nodiscard]]
		const Token* nextToken(std::size_t offset) const;
    	void error(const Token* token = nullptr);

    	std::vector<ska::Token> m_input;
    	std::size_t m_lookAheadIndex {};
    	Token* m_lookAhead {};
	};
}
