#pragma once

#include "LogEntry.h"
#include "LoggerImpl.h"
#include "LogLevel.h"

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
    
	template <class Wrapped>
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
                return LogEntry{ *this, logLevel };
            } else {
                return loggerdetail::EmptyProxy{};
            }   
        }

        ~Logger() = default;
    };
}

