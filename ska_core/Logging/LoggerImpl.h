#pragma once

#include <vector>
#include <unordered_map>

#include "LogLevel.h"
#include "Tokenizer.h"
#include "LogTarget.h"

namespace ska {

    namespace loggerdetail {
	    class LogEntry;
        class Logger {
            friend class LogEntry;
        
        protected:
            Logger(std::string className);
            Logger(std::string className, std::ostream& output);
        
        public:
            Logger(const Logger&) = delete;
            Logger(Logger&&) = default;

            void configureLogLevel(const LogLevel& logLevel) {
                m_logLevel = logLevel;
            }
        
            void setPattern(LogLevel logLevel, std::string pattern) {
                auto existingLoglevel = m_pattern.find(logLevel);
                if(existingLoglevel != m_pattern.end()) {
                    m_pattern.erase(existingLoglevel);
                }
                m_pattern.emplace(logLevel, Tokenizer {std::move(pattern)});
            }

            void addOutputTarget(std::ostream& output) {
                m_output.emplace_back(output);
            }

            virtual ~Logger() = default;

        private:
            template <class Arg>
            void pushMessage(Arg&& item) {
                for(auto& o : m_output) {
                    o << item;
                }
            }

            LogLevel m_logLevel;
            const std::string m_className;
            std::vector<LogTarget> m_output;
            std::unordered_map<LogLevel, Tokenizer> m_pattern;
            
            template <class T>
			friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);
        };
        
    }
}
