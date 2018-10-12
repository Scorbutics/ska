#pragma once

#include <vector>
#include <unordered_map>

#include "LogLevel.h"
#include "Tokenizer.h"
#include "LogTarget.h"

namespace ska {
    class LogEntry;

    template <class T>
    const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);

    namespace loggerdetail {
        class Logger {
            friend class ska::LogEntry;
        
        protected:
            Logger(std::string className);
            Logger(std::string className, std::ostream& output, LogFilter filter);
        
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

            void addOutputTarget(std::ostream& output, LogFilter filter) {
                m_output.emplace_back(output, filter);
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
			friend const ska::LogEntry& ska::operator<<(const ska::LogEntry& logEntry, T&& logPart);
        };
        
    }
}
