#include <cassert>

//strlen
#include <cstring>

//setfill, setw
#include <iomanip>

#include "LogTarget.h"
#include "LogEntry.h"
#include "ColorStream.h"

bool ska::loggerdetail::LogTarget::isATarget(const ska::LogEntry& entry) {
    return m_filter(entry);
}

namespace ska {
	namespace loggerdetail {
		std::size_t ExtractFileName(const char* path) {
			const auto length = strlen(path);
			int index;
			for(index = length - 1; index >= 0 && !(path[index] == '\\' || path[index] == '/'); index--);
			return static_cast<std::size_t>(index + 1);
		}
	}
}

ska::loggerdetail::TokenConsumeType ska::loggerdetail::LogTarget::applyTokenOnOutput(const ska::LogEntry& entry, const Token& token) {
    auto& output = m_output;
    const auto& date = entry.getDate(); 

	switch(token.type()) {
		case TokenType::Color :
			if(m_supportsColoring) {
				output << (static_cast<EnumColorStream>(token.length()));
			}
			break;
		
		case TokenType::Value :
			if(!m_supportsComplexLogging) {
				output << entry.getMessage();
			} else {
				return entry.isPatternReccursionFirstLevel() ? TokenConsumeType::ComplexPattern : TokenConsumeType::Consumed;
			}
			break;

		case TokenType::Year :
			output << (date.date.tm_year + 1900);
			break;
		
		case TokenType::Month :
			output << std::setfill('0') << std::setw(2) << (date.date.tm_mon + 1);
			break;
		
		case TokenType::Day :
			output << std::setfill('0') << std::setw(2) << date.date.tm_mday;
			break;
		
		case TokenType::Hour :
			output << std::setfill('0') << std::setw(2) << date.date.tm_hour;
			break;
		
		case TokenType::Minute :
			output << std::setfill('0') << std::setw(2) << date.date.tm_min;
			break;
		
		case TokenType::Second :
			output << std::setfill('0') << std::setw(2) << date.date.tm_sec;
			break;
	
        case TokenType::MilliSecond :
            output << std::setfill('0') << std::setw(3) << date.milliseconds;
            break;

        case TokenType::Identifier:
            output << &entry;
            break;

		case TokenType::Class:
            output << std::setfill(' ') << std::setw(token.length()) << entry.getContext().className.substr(0, token.length());
			break;
	    
        case TokenType::File:
            output << std::setfill(' ') << std::setw(token.length()) << std::string(entry.getContext().file + ExtractFileName(entry.getContext().file)).substr(0, token.length());
		break;
        
        case TokenType::Function:
            output << std::setfill(' ') << std::setw(token.length()) << std::string(entry.getContext().function).substr(0, token.length());
            break;

        case TokenType::Line:
            output << std::setfill('0') << std::setw(token.length()) << entry.getContext().line;
            break;

		case TokenType::Literal:
		    output << token.value();
			break;

		case TokenType::Empty:
			break;

		default:
			assert(false);
	}
	return TokenConsumeType::Consumed;
}

void ska::loggerdetail::LogTarget::endLine() {
	m_output << '\n';
}

