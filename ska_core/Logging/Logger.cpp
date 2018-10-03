#include <sstream>
#include "Logger.h"

unsigned int ska::LoggerFactory::m_classNameMaxLength = 20;
std::unique_ptr<ska::LoggerGlobalFormatter> ska::LoggerFactory::m_formatter = std::make_unique<LoggerGlobalFormatter>();

ska::loggerdetail::Tokenizer::Tokenizer(const std::string& str) {
    m_tokens = parse(str);
}

std::vector<ska::loggerdetail::Token> ska::loggerdetail::Tokenizer::parse(const std::string& str) {
    auto tokens = std::vector<Token> {};
    
    //auto currentTokenValue = std::stringstream {};
    //auto currentToken = Token {};
    for(auto i = 0u; i < str.size(); i++) {
        if(str[i] == '%') {
            tokens.push_back(parsePlaceholder(str, i));
        } else {
            tokens.push_back(parseLiteral(str, i));
        }
    }

    return tokens;
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parsePlaceholder(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    //we avoid scanning the percent symbol '%'
    index++;

    optionalNumeric();
    symbol();

    
}

ska::loggerdetail::Token ska::loggerdetail::Tokenizer::parseLiteral(const std::string& str, std::size_t& index) {
    auto tokenValue = std::stringstream {};

    for(;index < str.size() && str[index] != '%'; index++) {
        tokenValue << str[index];
    }

    return {tokenValue, TokenType::Literal};
}

