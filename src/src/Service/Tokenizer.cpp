#include <algorithm>
#include <iostream>

#include "Config/LoggerConfigLang.h"
#include "Error/ParserError.h"
#include "Tokenizer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Tokenizer)

const std::unordered_set<std::string> ska::Tokenizer::ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS = BuildAllowedMultipleCharTokenSymbolsSet();

std::unordered_set<std::string> ska::Tokenizer::BuildAllowedMultipleCharTokenSymbolsSet() {
	auto result = std::unordered_set<std::string> {};
	result.insert("+=");
	result.insert("-=");
	result.insert("*=");
	result.insert("/=");
	result.insert("!=");
	result.insert("==");
	result.insert(">=");
	result.insert("<=");
	result.insert("++");
	result.insert("--");
	result.insert("&=");
	result.insert("|=");
	return result;
}

ska::Tokenizer::Tokenizer(const ReservedKeywordsPool& reserved, std::string input) :
	m_reserved(reserved),
	m_input(std::move(input)) {
}

std::pair<ska::Token, ska::Token> ska::Tokenizer::stackToken(TokenType currentType, Token token, std::vector<Token>& stackSymbol) const {
	if (currentType == ska::TokenType::SYMBOL) {
		const auto charToken = token.name()[0];
		switch (isFinalizedSymbol(charToken, stackSymbol)) {
		case SymbolFinalizeTokenState::READY:
			stackSymbol.push_back(std::move(token));
			return {};
		case SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL: {
			auto result = std::make_pair(group(stackSymbol), Token{});
			stackSymbol.push_back(std::move(token));
			return result;
		}
		case SymbolFinalizeTokenState::FINALIZED_WITH_NEXT_SYMBOL:
			stackSymbol.push_back(std::move(token));
			return std::make_pair(group(stackSymbol), Token{});			
		default:
			assert(!"Unhandled token finalization state");
			return {};
		} 
	}

	return std::make_pair(group(stackSymbol), std::move(token));
}

ska::Cursor ska::Tokenizer::computeTokenPositionCursor(std::size_t index, const std::string& readToken, TokenGrammar grammar, bool wasRequired, const Cursor& lastCursor) const {
	const auto rawIndex = (wasRequired ? 0 : 1 ) + index;
	const auto isEOL = grammar == TokenGrammar::STATEMENT_END;

	ColumnIndex column = 0;
	LineIndex line = 0;

	if (isEOL) {
		column = 1u;
		line = lastCursor.line + static_cast<LineIndex>(std::count(readToken.begin(), readToken.end(), '\n'));
	} else {
		const auto tokenLength = readToken.size();
		column =
			(lastCursor.line == 1 ?
			(static_cast<unsigned int>(rawIndex) + 1) :
				lastCursor.column + static_cast<unsigned int>(tokenLength));

		line = lastCursor.line;
	}

	SLOG_STATIC(LogLevel::Debug, Tokenizer) << "Token calculated \"" << readToken << "\" at (l." << line << ", c. " << column << ")";

	return { 
		rawIndex, 
		column,
		line
	};
}

std::vector<ska::Token> ska::Tokenizer::tokenize() const {
	auto currentTokenToRead = RequiredToken {};
	auto stackSymbol = std::vector<Token> {};
	auto result = std::vector<Token>{};

	auto cursor = Cursor {};
	auto token = Token {};
	do {
		currentTokenToRead = determineCurrentToken(cursor.rawIndex);
		std::tie(cursor, token) = tokenizeNext(currentTokenToRead, cursor);
		if (isNotBlankToken(currentTokenToRead.current)) {	
			auto group = stackToken(currentTokenToRead.current, std::move(token), stackSymbol);
			push(std::move(group.first), result);
			push(std::move(group.second), result);			
		}
	
	} while (currentTokenToRead.current != ska::TokenType::EMPTY);
	
	push(group(stackSymbol), result);
	
	return result;
}

ska::Token ska::Tokenizer::group(std::vector<Token>& symbolStack) const {
	if (symbolStack.empty()) {
		return {};
	}

	auto ss = std::stringstream {}; 
	for(const auto& s : symbolStack) {
		ss << s;
	}
	auto tokenValueStr = ss.str();
	SLOG_STATIC(LogLevel::Info, Tokenizer) << "Pushing compound symbol \"" <<  tokenValueStr << "\"";

	auto grammar = m_reserved.grammar(tokenValueStr);
	auto symbolToken = ska::Token {std::move(tokenValueStr) , TokenType::SYMBOL, symbolStack.empty() ? Cursor {} : symbolStack[0].position(), grammar };
	symbolStack.clear();
	return symbolToken;
	
}

void ska::Tokenizer::push(Token t, std::vector<Token>& output) const {
	if (!t.empty()) {
		output.push_back(std::move(t));
	}
}

ska::RequiredToken ska::Tokenizer::determineCurrentToken(const std::size_t startIndex) const {
	if (startIndex < m_input.size()) {
		const auto charTokenType = calculateCharacterTokenType(m_input[startIndex]);
		const auto requiredToken = initializeCharType(charTokenType);
		return requiredToken;
	}
	return RequiredToken{};
}

//Handles "++", "--", "/=", "-=", "*=", ... has an unique token of symbol type instead of 2
ska::SymbolFinalizeTokenState ska::Tokenizer::isFinalizedSymbol(char nextSymbol, const std::vector<Token>& symbolStack) {
	if(symbolStack.size() >= 2) {
		return SymbolFinalizeTokenState::FINALIZED_WITH_NEXT_SYMBOL;
	}
	
	if(symbolStack.empty()) {
		return SymbolFinalizeTokenState::READY;
	}

	const auto topStackSymbol = symbolStack[0].name()[0];
	if(stopSymbolCharAggregation(topStackSymbol) < 0) {
		return SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL;
	}

	if(stopSymbolCharAggregation(nextSymbol) < 1) {
		return SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL;
	}

	auto symbol = std::string{};
	symbol.resize(2);
	symbol[0] = topStackSymbol;
	symbol[1] = nextSymbol;
	SLOG_STATIC(LogLevel::Debug, Tokenizer) << "Stack symbol : " << symbol;
	const auto result = ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS.find(symbol) != ALLOWED_MULTIPLE_CHAR_TOKEN_SYMBOLS.end();
	SLOG_STATIC(LogLevel::Debug, Tokenizer) << " which is " << (result ? "correct" : "incorrect");
	return result ? SymbolFinalizeTokenState::FINALIZED_WITH_NEXT_SYMBOL : SymbolFinalizeTokenState::FINALIZED_NO_NEXT_SYMBOL;
}

int ska::Tokenizer::stopSymbolCharAggregation(char symbol) {
	switch(symbol) {
		case '+':
		case '-':
		case '=':
			return 1;
		case '/':
		case '*':
		case '!':
		case '<':
		case '>':
		case '&':
		case '|':
			return 0;
		default:
			return -1;
	}
}

static std::string rtrim(std::string str) {
	static const auto whiteSpacesExceptEndLine = std::string(" \f\r\t\v");
	std::string::size_type pos = str.find_last_not_of(whiteSpacesExceptEndLine);
	if (pos == std::string::npos) {
		return "";
	}
	if (str[pos] == '\n') {
		return "\n";
	}
	return str.substr(0, pos + 1);
}

std::pair<ska::Cursor, ska::Token> ska::Tokenizer::tokenizeNext(const ska::RequiredToken& requiredToken, const Cursor& lastCursor) const {
	auto index = lastCursor.rawIndex == 0 ? 0u : lastCursor.rawIndex + 1;
	if (index < m_input.size()) {
		auto charTokenType = requiredToken.current;
		for (; index < m_input.size(); index++) {
			charTokenType = calculateCharacterTokenType(m_input[index]);
			if (index > lastCursor.rawIndex && requiredToken.required ^ requiredToken.requiredOrUntil[static_cast<std::size_t>(charTokenType)]) {
				if (index == 0) {
					index++;
				}
				break;
			}
		}
	} else {
		index = m_input.size();
	}
	
	// Preserve unnecessary things (as comments, too much end line characters ...) to keep track of the real token position in file (Cursor) !
	// Then, when we have the token position in file, we trim the token value and deduce its real value
	auto tokenValueRawInput = finalizeRawToken(index, requiredToken, lastCursor);
	auto finalTokenValue = requiredToken.current == TokenType::STRING ? tokenValueRawInput : rtrim(tokenValueRawInput);
	
	auto tokenGrammar = m_reserved.grammar(finalTokenValue);
	auto nextCursor = computeTokenPositionCursor(index, tokenValueRawInput, tokenGrammar, requiredToken.required, lastCursor);
	
	Token token;
	if (requiredToken.current == TokenType::IDENTIFIER && m_reserved.pool.find(finalTokenValue) != m_reserved.pool.end()) {
		token = Token{ m_reserved.pool.at(finalTokenValue).token, nextCursor };
	} else {
		token = Token{ std::move(finalTokenValue), requiredToken.current, nextCursor, tokenGrammar };
	}
	return std::make_pair(std::move(nextCursor), std::move(token));
}

ska::RequiredToken ska::Tokenizer::initializeCharType(const ska::TokenType charTokenType) const {
	auto required = ska::RequiredToken{};
	required.current = charTokenType;
	switch (charTokenType) {
	case ska::TokenType::DOT_SYMBOL:
	case ska::TokenType::DIGIT:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DIGIT)] = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DOT_SYMBOL)] = true;
		break;

	case ska::TokenType::SPACE:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::SPACE)] = true;
		break;

	case ska::TokenType::SYMBOL:
		required.required = true;
		break;
	
	case ska::TokenType::END_STATEMENT:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::END_STATEMENT)] = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::SPACE)] = true;
		break;

	case ska::TokenType::RANGE:
		required.required = true;
		break;

	case ska::TokenType::ARRAY:
		required.required = true;
		break;

	case ska::TokenType::STRING:
		required.required = false;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::STRING)] = true;
		break;

	default:
	case ska::TokenType::IDENTIFIER:
		required.required = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::DIGIT)] = true;
		required.requiredOrUntil[static_cast<std::size_t>(ska::TokenType::IDENTIFIER)] = true;
		break;
	}
	return required;
}

std::string ska::Tokenizer::getInputStringTokenOrThrow(const TokenType& tokenType, std::size_t index, const Cursor& lastCursor, const std::size_t offset) const {
	if((lastCursor.rawIndex + offset) >= m_input.size()) {
		throw ParserError("unterminated token " + std::string{TokenTypeSTR[static_cast<std::size_t>(tokenType)]}, lastCursor);
	}

	return m_input.substr(lastCursor.rawIndex + offset, index - lastCursor.rawIndex - 2 * offset);
}

std::string ska::Tokenizer::postComputing(std::size_t index, const ska::RequiredToken& requiredToken, const Cursor& lastCursor) const {
	std::string value;
	switch (requiredToken.current) {
	case TokenType::STRING: 
		value = getInputStringTokenOrThrow(requiredToken.current, index, lastCursor, 1);
		break;

	default:
		value = getInputStringTokenOrThrow(requiredToken.current, index, lastCursor, 0);
		break;
		
	}

	return std::move(value);
}

std::string ska::Tokenizer::finalizeRawToken(std::size_t index, const ska::RequiredToken& requiredToken, const Cursor& lastCursor) const {
	index += (!requiredToken.required ? 1 : 0);
	if (index != lastCursor.rawIndex) {
		return postComputing(index, requiredToken, lastCursor);
	}
	return {};
}

ska::TokenType ska::Tokenizer::calculateCharacterTokenType(const char c) const {
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return ska::TokenType::DIGIT;
	
	case '.':
		return ska::TokenType::DOT_SYMBOL;

	case '\n':
		return ska::TokenType::END_STATEMENT;

	case ' ':
	case '\t':
	case '\v':
	case '\f':
	case '\r':
		return ska::TokenType::SPACE;

	case '"':
		return ska::TokenType::STRING;

	case '(':
	case ')':
	case '{':
	case '}':
		return ska::TokenType::RANGE;

	case '[':
	case ']':
		return ska::TokenType::ARRAY;

	default:
		return isWordCharacter(c) ? ska::TokenType::IDENTIFIER : ska::TokenType::SYMBOL;
	}
}
