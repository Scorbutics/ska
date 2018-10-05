#pragma once
#include <string>

namespace ska {
	namespace loggerdetail {
        enum class TokenType {
            Color,
			Class,
            Value,
            Year,
            Month,
            Day,
            Hour,
            Minute,
            Second,
            Literal,
            Empty
        };

        struct Token {
        public:
			Token() = default;
            Token(std::string value, TokenType type, std::size_t length = 0) :
				m_value(std::move(value)),
				m_type(type), 
				m_length(length) {
				if(m_length == 0) {
					m_length = m_value.size();
				}
			}
		
            std::size_t length() const {
                return m_length;
            }

            TokenType type() const {
                return m_type;
            }
            
            const std::string& value() const {
                return m_value;
            }

		private:
			std::string m_value;
            TokenType m_type = TokenType::Empty;
			std::size_t m_length = 0;
        };
	}
}