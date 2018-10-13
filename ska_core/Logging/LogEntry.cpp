#include <ctime>
#include <cassert>
#include <iomanip>
#include "LogEntry.h"
#include "ColorStream.h"

tm ska::LogEntry::currentDateTime() {
	auto t = std::time(nullptr);
#ifdef _MSC_VER
	tm buf;
	localtime_s(&buf, &t);
#else
	tm buf = *std::localtime(&t);
#endif
	return buf;
}

void ska::LogEntry::consumeTokens() {
    if(alreadyLogged) {
        return;
    }

    fullMessage << "\n";
    
    auto& currentPattern = instance->m_pattern.at(logLevel);
	
    for(auto& o : instance->m_output) {
        while(!currentPattern.empty()) {
            const auto& token = currentPattern.next();
            if(!o.applyTokenOnOutput(*this, token)) {
                break;
            }
        }
        currentPattern.rewind();
    }

    alreadyLogged = true;
}




