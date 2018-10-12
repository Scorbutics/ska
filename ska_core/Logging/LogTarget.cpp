#include <iomanip>
#include <cassert>

#include "LogTarget.h"
#include "LogEntry.h"
#include "ColorStream.h"



bool ska::loggerdetail::LogTarget::isATarget(const LogEntry& entry) {
    return m_filter(entry);
}

bool ska::loggerdetail::LogTarget::applyTokenOnOutput(const LogEntry& entry, const Token& token) {
    if(!isATarget(entry)) {
        return false;
    }

    auto& output = m_output;
    const tm& date = entry.getDate(); 
    const std::string& logMessage = entry.getMessage();

	switch(token.type()) {
		case TokenType::Color :
			output << (static_cast<EnumColorStream>(token.length()));
			break;
		
		case TokenType::Value :
			output << logMessage;
			break;

		case TokenType::Year :
			output << std::put_time(&date, "%Y");
			break;
		
		case TokenType::Month :
			output << std::put_time(&date, "%m");
			break;
		
		case TokenType::Day :
			output << std::put_time(&date, "%d");
			break;
		
		case TokenType::Hour :
			output << std::put_time(&date, "%H");
			break;
		
		case TokenType::Minute :
			output << std::put_time(&date, "%M");
			break;
		
		case TokenType::Second :
			output << std::put_time(&date, "%S");
			break;
		
		case TokenType::Class:
			output << entry.getClassName();
			break;
		
		case TokenType::Literal:
		    output << token.value();
			break;

		case TokenType::Empty:
			break;

		default:
			assert(false);
	}
    return true;
}

