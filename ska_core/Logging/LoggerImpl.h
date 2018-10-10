#pragma once

//TODO reference_wrapper
#include <functional>

#include <vector>
#include <unordered_map>

#include "Tokenizer.h"

namespace ska {
    enum class LogLevel {
		Debug = 0,
		Info = 1,
		Warn = 2,
		Error = 3,
		Disabled = 100
	};

    namespace loggerdetail {
	    class LogEntry;
        class Logger {
            friend class LogEntry;
        
        private:
            Logger(const Logger&) = delete;

        protected:
            Logger(std::string className);
            Logger(std::string className, std::ostream& output);
        
        public:
            Logger(Logger&&) = default;

            void configureLogLevel(const LogLevel& logLevel) {
                m_logLevel = logLevel;
            }
        
            void setPattern(LogLevel logLevel, std::string pattern) {
                auto existingLoglevel = m_pattern.find(logLevel);
                if(existingLoglevel != m_pattern.end()) {
                    m_pattern.erase(existingLoglevel);
                }
                m_pattern.emplace(logLevel, loggerdetail::Tokenizer {std::move(pattern)});
            }

            void addOutputTarget(std::ostream& output) {
                m_output.push_back(output);
            }

            virtual ~Logger() = default;

        private:
            template <class Arg>
            void pushMessage(Arg&& item) {
                for(auto& o : m_output) {
                    (o.get()) << std::forward<Arg>(item);
                }
            }

            LogLevel m_logLevel;
            const std::string m_className;
            std::vector<std::reference_wrapper<std::ostream>> m_output;
            std::unordered_map<LogLevel, loggerdetail::Tokenizer> m_pattern;
            
            template <class T>
			friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);
        };
        
    }
}
