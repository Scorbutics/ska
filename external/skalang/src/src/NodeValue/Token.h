#pragma once
#include <string>
#include <variant>
#include <sstream>
#include <cassert>
#include <memory>

#include "TokenGrammar.h"
#include "Cursor.h"

namespace ska {
	class MemoryTable;
	class ASTNode;
	constexpr const char* TokenTypeSTR[] = {
		"RESERVED",
		"IDENTIFIER",
		"DIGIT",
		"DOT_SYMBOL",
		"END_STATEMENT",
		"SPACE",
		"STRING",
		"RANGE",
		"ARRAY",
		"SYMBOL",
		"BOOLEAN",
		"EMPTY",
		"UNUSED_LAST_Length"
	};

	enum class TokenType {
		RESERVED,
		IDENTIFIER,
		DIGIT,
		DOT_SYMBOL,
		END_STATEMENT,
		SPACE,
		STRING,
		RANGE,
		ARRAY,
		SYMBOL,
		BOOLEAN,
		EMPTY,
		UNUSED_LAST_Length
	};

	struct Token {
		using Variant = std::variant<std::size_t, std::string>;
		Token() = default;
		Token(const Token& t) = default;
		Token(Token&& t) noexcept = default;
		Token& operator=(const Token& t) = default;
		Token& operator=(Token&& t) noexcept = default;

		Token(const Token& t, Cursor position) :
			Token(t) {
			m_position = std::move(position);
		}

		Token(std::string c, TokenType t, Cursor position, TokenGrammar grammar = TokenGrammar::UNUSED_Last_Length) :
			m_type(std::move(t)),
			m_position(std::move(position)),
			m_grammar(grammar) {
			init(c, m_type);
		}

		Token(std::size_t c, TokenType t, Cursor position, TokenGrammar grammar) :
			m_type(std::move(t)),
			m_position(std::move(position)),
			m_grammar(grammar) {
			if (m_type == TokenType::EMPTY) {
				m_content = "";
			} else {
				init(c, m_type);
			}
		}

		const auto& content() const {
			return m_content;
		}

		const TokenType& type() const {
			return m_type;
		}

    	bool isLiteral() const {
        	switch(m_type) {
				case TokenType::BOOLEAN:
            	case TokenType::DIGIT:
            	case TokenType::STRING:
                	return true;
            	default:
                	return false;
        	}
    	}
		
		std::string name() const {
			if (m_type == TokenType::RESERVED) {
				return TokenGrammarSTR[std::get<std::size_t>(m_content)];
			}
			return std::holds_alternative<std::string>(m_content) ? std::get<std::string>(m_content) : "";
		}

		bool empty() const {
			return m_type == TokenType::EMPTY;
		}

		const Cursor& position() const {
			return m_position;
		}

		TokenGrammar grammar() const {
			return m_grammar;
		}

		bool operator==(const Token& t1) const {
			return m_type == t1.m_type && m_content == t1.m_content;
		}

		bool operator!=(const Token& t1) const {
			return m_type != t1.m_type || m_content != t1.m_content;
		}

		//void print(std::ostream& stream, const ReservedKeywordsPool& pool);

	private:
		void init(std::size_t c, TokenType t) {
			m_type = t;
			assert(m_type == TokenType::RESERVED);
			m_content = c;
		}

		void init(std::string c, TokenType t) {
			m_type = t;
			assert(m_type != TokenType::RESERVED && m_type != TokenType::UNUSED_LAST_Length);
			m_content = std::move(c);
		}

		Variant m_content = std::string{};
		TokenType m_type = TokenType::EMPTY;
		TokenGrammar m_grammar = TokenGrammar::UNUSED_Last_Length;
		Cursor m_position;

		friend std::ostream& operator<<(std::ostream& stream, const Token& token);
	};

	std::ostream& operator<<(std::ostream& stream, const Token& token);

}
