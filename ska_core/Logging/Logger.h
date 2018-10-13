#pragma once

#include "LogEntry.h"
#include "LoggerImpl.h"
#include "LogLevel.h"

#include "LogSync.h"

namespace ska {

	template<class T>
	class LoggerClassFormatter;
	
	template<class T>
	class LoggerClassLevel {
    public:
        static constexpr const auto level = LogLevel::Debug;
    };

    namespace loggerdetail {
        struct EmptyProxy {};
        template <class T>
        const EmptyProxy& operator<<(const EmptyProxy& logEntry, T&& logPart) {
            return logEntry;
        }
    }
    
	template <class Wrapped, class LogMethod = LogSync>
    class Logger : public loggerdetail::Logger {
    public:
        Logger(std::ostream& output, LogFilter filter) :
            loggerdetail::Logger(std::move(LoggerClassFormatter<Wrapped>::format()), output, std::move(filter)) {
        }
        
        Logger() :
            loggerdetail::Logger(std::move(LoggerClassFormatter<Wrapped>::format())) {
        }

        template<LogLevel logLevel>
        auto log() {
            if constexpr (logLevel >= LoggerClassLevel<Wrapped>::level) {
                return loggerdetail::LogEntry<LogMethod>{ *this, logLevel, m_logMethod };
            } else {
                return loggerdetail::EmptyProxy{};
            }   
        }

        ~Logger() = default;
    
    private:
        LogMethod m_logMethod;
    };
}

