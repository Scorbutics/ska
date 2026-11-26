#pragma once
#include <vector>
#include "Token.h"

namespace ska {
	namespace loggerdetail {
		
		class Tokenizer {
        public:
            Tokenizer(const std::string& str);
            ~Tokenizer() = default;

			auto cbegin() const noexcept {
				return m_tokens.cbegin();
			}

			auto cend() const noexcept {
				return m_tokens.cend();
			}

			auto begin() const noexcept {
				return m_tokens.cbegin();
			}

			auto end() const noexcept {
				return m_tokens.cend();
			}

			std::size_t size() const {
				return m_tokens.size();
			}
			
			std::size_t empty() const {
				return m_tokens.empty();
			}
			
        private:
            static std::vector<Token> parse(const std::string& str);
            
            static Token parseLiteral(const std::string& str, std::size_t& index);
			static Token parsePlaceholder(const std::string& str, std::size_t& index);
			
            static std::size_t matchOptionalNumeric(const std::string& str, std::size_t& index);
            static TokenType matchCharacterNonNumeric(const std::string& str, std::size_t& index);

			std::vector<Token> m_tokens;
        };
	}
}
