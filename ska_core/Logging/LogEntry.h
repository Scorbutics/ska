#pragma once
#include <sstream>

#include "LoggerImpl.h"

namespace ska {
    namespace loggerdetail {
        class LogEntry {
		public:
            LogEntry(Logger& instance, LogLevel logLevel) : 
                instance(instance), 
                logLevel(logLevel), 
                date(currentDateTime()) {
            }

            ~LogEntry() {
				//MUST NOT throw !
                consumeTokens(instance, date, message.str());
			}

        private:
			Logger& instance;
			LogLevel logLevel;
			
            static void applyTokenOnOutput(Logger& instance, const struct tm& date, const Token& token, const std::string& message);
            static void consumeTokens(Logger& instance, const tm& date, const std::string& messages);

            //Mutable used because LogEntry is only a short time wrapper-class that is destroyed at the end of the log line
            mutable std::stringstream message;
            tm date;
            static tm currentDateTime();
		public:
			template <class T>
			friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);	
		};

        template <class T>
        const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart) {
            if (logEntry.logLevel >= logEntry.instance.m_logLevel) {
                logEntry.message << std::forward<T>(logPart);
            }
            return logEntry;
        }

        
    }
}
