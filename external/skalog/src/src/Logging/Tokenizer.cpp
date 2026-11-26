#include <sstream>
#include <cassert>

#include "ColorStream.h"
#include "Logger.h"

ska::loggerdetail::Tokenizer::Tokenizer(const std::string& str) {
    m_tokens = parse(str);
}

std::vector<ska::loggerdetail::Token> ska::loggerdetail::Tokenizer::parse(const std::string& str) {
    auto tokens = std::vector<Token> {};
    
	try {
		for(std::size_t i = 0u; i < str.size(); i++) {
			if(str[i] == '%') {
				tokens.push_back(parsePlaceholder(str, i));
			} else {
				tokens.push_back(parseLiteral(str, i));
			}
		}
	} catch (std::runtime_error& ex) {
		auto ss = std::stringstream{};
		ss << "Error while parsing the log pattern : " << ex.what();
		throw std::runtime_error(ss.str());
	}

    return tokens;
}

std::size_t ska::loggerdetail::Tokenizer::matchOptionalNumeric(const std::string& str, std::size_t& index) {	
	auto numericValue = std::stringstream {};
	
	
	auto isIntValue = str[index] >= '0' && str[index] <= '9';
	while(index < str.size() && isIntValue) {
		numericValue << str[index];
		index++;
		isIntValue = str[index] >= '0' && str[index] <= '9';
	}
	
	if (numericValue.str().empty()) {
		return 0;
	}
	
	auto iss = std::istringstream{ numericValue.str() };
	std::size_t result;
	iss >> result;
	return result;
}

ska::loggerdetail::TokenType ska::loggerdetail::Tokenizer::matchCharacterNonNumeric(const std::string& str, std::size_t& index) {	
	if(index >= str.size()) {
		throw std::runtime_error("unexpected early end of input");
	}
	
    const auto isIntValue = str[index] >= '0' && str[index] <= '9';
	if(!isIntValue) {
	    auto ss = std::stringstream {};
        ss << str[index];
        auto characterSymbol = str[index];
        auto tokenType = TokenType{};
        switch(characterSymbol) {
			case 'C':
                tokenType = TokenType::Class;
                break;
            case 'c':
                tokenType = TokenType::Color;
                break;
            case 'i':
                tokenType = TokenType::Identifier;
                break;
            case 'v':
                tokenType = TokenType::Value;
                break;
            case 'y':
                tokenType = TokenType::Year;
                break;
            case 'M':
                tokenType = TokenType::Month;
                break;
            case 'd':
                tokenType = TokenType::Day;
                break;
            case 'h':
                tokenType = TokenType::Hour;
                break;
            case 'm':
                tokenType = TokenType::Minute;
                break;
            case 's':
                tokenType = TokenType::Second;
                break;
            case 'l':
                tokenType = TokenType::Line;
                break;
            case 'f':
                tokenType = TokenType::Function;
                break;
            case 'F':
                tokenType = TokenType::File;
                break;
            case 'T':
                tokenType = TokenType::MilliSecond;
                break;
            default:
                throw std::runtime_error("unknown symbol : " + ss.str());
        }
        return tokenType;

	} else {
        auto ss = std::stringstream {};
        ss << str[index];
        throw std::runtime_error("expected a non numeric character but found \"" + ss.str() + "\" instead");    
    }
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parsePlaceholder(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    //we avoid scanning the percent symbol '%'
    index++;

    const auto tokenLength = matchOptionalNumeric(str, index);
    const auto tokenType = matchCharacterNonNumeric(str, index);
	
    return Token{ "", tokenType, tokenLength };
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parseLiteral(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    for(;index < str.size() && str[index] != '%'; index++) {
        tokenValue << str[index];
    }
	index--;
	
    return Token{tokenValue.str(), TokenType::Literal};
}



