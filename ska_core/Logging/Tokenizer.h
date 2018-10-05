#pragma once

#include "Token.h"

namespace ska {
	namespace loggerdetail {
		class Tokenizer {
        public:
            Tokenizer(const std::string& str);
            ~Tokenizer() = default;
            
            const Token& next() {
                return m_cursor < m_tokens.size() ? m_tokens[m_cursor++] : m_emptyToken;
            }

            bool empty() const {
                return m_cursor >= m_tokens.size();
            }

            void rewind() {
                m_cursor = 0;
            }

        private:
            static std::vector<Token> parse(const std::string& str);
            
            static Token parseLiteral(const std::string& str, std::size_t& index);
			static Token parsePlaceholder(const std::string& str, std::size_t& index);
			
            static std::size_t matchOptionalNumeric(const std::string& str, std::size_t& index);
            static std::pair<std::string, TokenType> matchCharacterNonNumeric(const std::string& str, std::size_t& index);

			std::vector<Token> m_tokens;
            std::size_t m_cursor = 0;
            Token m_emptyToken;
        };
	}
}