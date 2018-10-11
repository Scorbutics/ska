#pragma once

#include <sstream>
#include "LogLevel.h"
#include "Tokenizer.h"
#include "LoggerImpl.h"

namespace ska {
    namespace loggerdetail {
        class LogTarget;

        class LogEntry {
        public:
            LogEntry(Logger& instance, LogLevel logLevel) : 
                instance(instance), 
                logLevel(logLevel), 
                date(currentDateTime()),
                className(instance.m_className) {
            }
            
            LogEntry(const LogEntry&) = delete;
            LogEntry(LogEntry&&) = default;

            ~LogEntry() {
                //MUST NOT throw !
                fullMessage << "\n";
                consumeTokens();
            }

            const std::string& getClassName() const {
                return className;
            }

            const LogLevel& getLogLevel() const {
                return logLevel;
            }

            std::string getMessage() const {
                return fullMessage.str();
            }

            const tm& getDate() const {
                return date;
            }

        private:
			Logger& instance;
			LogLevel logLevel;
			//Mutable used because LogEntry is only a short time wrapper-class that is destroyed at the end of the log line
            mutable std::stringstream fullMessage;
            tm date;
            std::string className;
            
            void consumeTokens();

            static tm currentDateTime();
			
            template <class T>
			friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);	
        };

        template <class T>
        const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart) {
            if (logEntry.logLevel >= logEntry.instance.m_logLevel) {
                logEntry.fullMessage << std::forward<T>(logPart);
            }
            return logEntry;
        }

        
    }
}
