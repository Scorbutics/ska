#include <sstream>
#include <iomanip>
#include <cassert>

#include "ColorStream.h"
#include "Logger.h"
#include "../Utils/StringUtils.h"

unsigned int ska::LoggerFactory::m_classNameMaxLength = 20;

ska::loggerdetail::Tokenizer::Tokenizer(const std::string& str) {
    m_tokens = parse(str);
}

std::vector<ska::loggerdetail::Token> ska::loggerdetail::Tokenizer::parse(const std::string& str) {
    auto tokens = std::vector<Token> {};
    
	try {
		for(auto i = 0u; i < str.size(); i++) {
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
	
	return numericValue.str().empty() ? 0 : ska::StringUtils::fromString<std::size_t>(numericValue.str());
}

std::pair<std::string, ska::loggerdetail::TokenType> ska::loggerdetail::Tokenizer::matchCharacterNonNumeric(const std::string& str, std::size_t& index) {	
	if(index >= str.size()) {
		throw std::runtime_error("unexpected early end of input");
	}
	
	auto ss = std::stringstream {};
	
	if(!ska::StringUtils::isInt(&str[index], 10)) {
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

            default:
                throw std::runtime_error("unknown symbol : " + ss.str());
        }
        return std::make_pair(ss.str(), tokenType);

	} else {
        throw std::runtime_error("expected a non numeric character but found \"" + ss.str() + "\" instead");    
    }
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parsePlaceholder(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    //we avoid scanning the percent symbol '%'
    index++;

    const auto tokenLength = matchOptionalNumeric(str, index);
    const auto [tokenSymbol, tokenType] = matchCharacterNonNumeric(str, index);
	
    return Token{ tokenSymbol, tokenType, tokenLength };
}

struct tm ska::Logger::currentDateTime() {
	auto t = std::time(nullptr);
#ifdef _MSC_VER
	struct tm buf;
	localtime_s(&buf, &t);
#else
	struct tm buf = *std::localtime(&t);
#endif
	return buf;
}

void ska::Logger::consumeTokens(const tm& date, const std::string& message) {
    
	auto& currentPattern = m_pattern.at(m_logLevel);
	while(!currentPattern.empty()) {
        applyTokenOnOutput(date, currentPattern.next(), message);
    }
	
	m_output << std::endl;
    currentPattern.rewind();
}

ska::loggerdetail::LogEntry ska::Logger::operator<<(const EnumLogLevel& logLevel) {
	return { *this, logLevel };
}

void ska::Logger::applyTokenOnOutput(const struct tm& date, const loggerdetail::Token& token, const std::string& message) {
	
	switch(token.type()) {
		case loggerdetail::TokenType::Color :
			m_output << static_cast<EnumColorStream>(token.length());
			break;
		
		case loggerdetail::TokenType::Value :
			m_output << message;
			break;

		case loggerdetail::TokenType::Year :
			m_output << std::put_time(&date, "%Y");
			break;
		
		case loggerdetail::TokenType::Month :
			m_output << std::put_time(&date, "%m");
			break;
		
		case loggerdetail::TokenType::Day :
			m_output << std::put_time(&date, "%d");
			break;
		
		case loggerdetail::TokenType::Hour :
			m_output << std::put_time(&date, "%H");
			break;
		
		case loggerdetail::TokenType::Minute :
			m_output << std::put_time(&date, "%M");
			break;
		
		case loggerdetail::TokenType::Second :
			m_output << std::put_time(&date, "%S");
			break;
		
		case loggerdetail::TokenType::Class:
			m_output << m_className;
			break;
		
		case loggerdetail::TokenType::Literal:
			m_output << token.value();
			break;

		case loggerdetail::TokenType::Empty:
			break;

		default:
			assert(false);
	}
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parseLiteral(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    for(;index < str.size() && str[index] != '%'; index++) {
        tokenValue << str[index];
    }
	index--;
	
    return Token{tokenValue.str(), TokenType::Literal};
}
