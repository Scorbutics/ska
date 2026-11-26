#include "Token.h"

std::ostream& ska::operator<<(std::ostream& stream, const Token& token) {
	if (token.m_type == TokenType::EMPTY) {
		return stream;
	}

	if (std::holds_alternative<std::string>(token.m_content)) {
		stream << std::get<std::string>(token.m_content);
		return stream;
	}

	if (token.m_type < TokenType::UNUSED_LAST_Length) {
		stream << TokenGrammarSTR[std::get<std::size_t>(token.m_content)];
	} else {
		stream << "error variant";
	}

	return stream;
}

/*
void print(std::ostream& stream, const ReservedKeywordsPool& pool) {

}
*/