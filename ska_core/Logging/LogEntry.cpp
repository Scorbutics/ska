#include <ctime>
#include <cassert>
#include <iomanip>
#include "LogEntry.h"
#include "ColorStream.h"

tm ska::loggerdetail::LogEntry::currentDateTime() {
	auto t = std::time(nullptr);
#ifdef _MSC_VER
	tm buf;
	localtime_s(&buf, &t);
#else
	tm buf = *std::localtime(&t);
#endif
	return buf;
}

void ska::loggerdetail::LogEntry::consumeTokens(Logger& instance, const tm& date, const std::string& message) {
	auto& currentPattern = instance.m_pattern.at(instance.m_logLevel);
	while(!currentPattern.empty()) {
        applyTokenOnOutput(instance, date, currentPattern.next(), message);
    }
	
	instance.pushMessage("\n");
    currentPattern.rewind();
}

void ska::loggerdetail::LogEntry::applyTokenOnOutput(Logger& instance, const struct tm& date, const loggerdetail::Token& token, const std::string& message) {
	
	switch(token.type()) {
		case loggerdetail::TokenType::Color :
			instance.pushMessage(static_cast<EnumColorStream>(token.length()));
			break;
		
		case loggerdetail::TokenType::Value :
			instance.pushMessage(message);
			break;

		case loggerdetail::TokenType::Year :
			instance.pushMessage(std::put_time(&date, "%Y"));
			break;
		
		case loggerdetail::TokenType::Month :
			instance.pushMessage(std::put_time(&date, "%m"));
			break;
		
		case loggerdetail::TokenType::Day :
			instance.pushMessage(std::put_time(&date, "%d"));
			break;
		
		case loggerdetail::TokenType::Hour :
			instance.pushMessage(std::put_time(&date, "%H"));
			break;
		
		case loggerdetail::TokenType::Minute :
			instance.pushMessage(std::put_time(&date, "%M"));
			break;
		
		case loggerdetail::TokenType::Second :
			instance.pushMessage(std::put_time(&date, "%S"));
			break;
		
		case loggerdetail::TokenType::Class:
			instance.pushMessage(instance.m_className);
			break;
		
		case loggerdetail::TokenType::Literal:
			instance.pushMessage(token.value());
			break;

		case loggerdetail::TokenType::Empty:
			break;

		default:
			assert(false);
	}
}


