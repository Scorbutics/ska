#include "Logger.h"

unsigned int ska::LoggerFactory::m_classNameMaxLength = 20;
std::unique_ptr<ska::LoggerGlobalFormatter> ska::LoggerFactory::m_formatter = std::make_unique<LoggerGlobalFormatter>();
