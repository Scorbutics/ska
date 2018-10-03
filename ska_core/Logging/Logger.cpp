#include <sstream>
#include <iomanip>
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
	while(index < str.size() && ska::StringUtils::isInt(str[index], 10)) {
		numericValue << str[index];
		index++;
	}
	
	return numericValue.empty() ? 0 : ska::StringUtils::fromString<std::size_t>(numericValue.str());
}

std::pair<std::string, ska::loggerdetail::TokenType> ska::loggerdetail::Tokenizer::matchCharacterNonNumeric(const std::string& str, std::size_t& index) {	
	if(index >= str.size()) {
		throw std::runtime_error("unexpected early end of input");
	}
	
	auto ss = std::stringstream {};
	if(!ska::StringUtils::isInt(str[index], 10)) {
		ss << str[index];
		return ss.str();
	}
	
	throw std::runtime_error("expected a non numeric character but found \"" + ss.str() + "\" instead");
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parsePlaceholder(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    //we avoid scanning the percent symbol '%'
    index++;

    const auto tokenLength = matchOptionalNumeric(str, index);
    const auto [tokenSymbol, tokenType] = matchCharacterNonNumeric(str, index);

    return Token{ tokenSymbol, tokenType, tokenLength };
}

void ska::Logger::printDateTime(std::ostream& os) {
				auto t = std::time(nullptr);
#ifdef _MSC_VER
			struct tm buf;
			localtime_s(&buf, &t);
#else
			struct tm buf = *std::localtime(&t);
#endif
			//std::cout << EnumColorStream::LIGHTMAGENTA;
			os << "[" << std::put_time(&buf, "%H:%M:%S") << "] ";
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parseLiteral(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    for(;index < str.size() && str[index] != '%'; index++) {
        tokenValue << str[index];
    }

    return Token{tokenValue.str(), TokenType::Literal};
}
