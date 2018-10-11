#include <iostream>

#include "LoggerImpl.h"

ska::loggerdetail::Logger::Logger(std::string className) :
    Logger(std::move(className), std::cout) {
}

ska::loggerdetail::Logger::Logger(std::string className, std::ostream& output) :
    m_logLevel(LogLevel::Debug),
    m_className(std::move(className)) {
    m_output.push_back(output);
    
    m_pattern.emplace(LogLevel::Debug, Tokenizer { "%10c[%h:%m:%s]%9c[Debug] %14c%C %15c%v" });
    m_pattern.emplace(LogLevel::Info, Tokenizer { "%10c[%h:%m:%s]%10c[Info] %14c%C %15c%v" });
    m_pattern.emplace(LogLevel::Warn, Tokenizer { "%10c[%h:%m:%s]%11c[Warn] %14c%C %15c%v" });
    m_pattern.emplace(LogLevel::Error, Tokenizer { "%10c[%h:%m:%s]%12c[Error] %14c%C %15c%v" });
}


