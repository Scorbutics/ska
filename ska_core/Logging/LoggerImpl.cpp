#include <iostream>

#include "LoggerImpl.h"

ska::loggerdetail::Logger::Logger(std::string className) :
    Logger(std::move(className), std::cout, GetIdentityLogFilter()) {
}

ska::loggerdetail::Logger::Logger(std::string className, std::ostream& output, LogFilter filter) :
    m_logLevel(LogLevel::Debug),
    m_className(std::move(className)) {
    m_output.emplace_back(output, std::move(filter));
    
    m_pattern.emplace(LogLevel::Debug, Tokenizer { "%10c[%h:%m:%s:%T]%9c[Debug] %14c%C %15c%v" });
    m_pattern.emplace(LogLevel::Info, Tokenizer { "%10c[%h:%m:%s:%T]%10c[Info] %14c%C %15c%v" });
    m_pattern.emplace(LogLevel::Warn, Tokenizer { "%10c[%h:%m:%s:%T]%11c[Warn] %14c%C %15c%v" });
    m_pattern.emplace(LogLevel::Error, Tokenizer { "%10c[%h:%m:%s:%T]%12c[Error] %14c%C %15c%v" });
}


