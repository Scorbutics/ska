#include <iostream>

#include "LoggerImpl.h"

ska::loggerdetail::Logger::Logger(std::string className) :
    Logger(std::move(className), std::cout) {
}

ska::loggerdetail::Logger::Logger(std::string className, std::ostream& output) :
    m_logLevel(LogLevel::Debug),
    m_className(std::move(className)),
    m_output(output) {
    
    const auto& defaultPattern = "%10c[%h:%m:%s] %14c%C %15c%v";
    m_pattern.emplace(LogLevel::Debug, Tokenizer { defaultPattern });
    m_pattern.emplace(LogLevel::Info, Tokenizer { defaultPattern });
    m_pattern.emplace(LogLevel::Error, Tokenizer { defaultPattern });
    m_pattern.emplace(LogLevel::Warn, Tokenizer { defaultPattern });
}


