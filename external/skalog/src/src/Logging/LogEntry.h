#pragma once

#include <sstream>
#include <cassert>
#include <optional>
#include <ctime>
#include <chrono>
#include "LogLevel.h"
#include "LogContext.h"
#include "Tokenizer.h"
#include "LoggerImpl.h"

namespace ska {
	
	namespace loggerdetail {
		struct LogTimePoint {
			tm date;
			std::size_t milliseconds;
		};
	}
	
    class LogEntry {
		using LogCallback = std::function<void(LogEntry&)>;

	public:
		LogEntry(LogCallback callback, loggerdetail::LogContext context, bool disabled = false) :
			context(std::move(context)),
			date(currentDateTime()),
			callback(callback),
			disabled(disabled) {
		}

		LogEntry(const LogEntry& e) {
			context = e.context;
			date = e.date;
			callback = e.callback;
			disabled = e.disabled;
			fullMessage << e.fullMessage.rdbuf();
		}

		LogEntry(LogEntry&& e) noexcept {
			context = e.context;
			date = e.date;
			e.callback.swap(callback);
			disabled = e.disabled;
			e.disabled = true;
			fullMessage << e.fullMessage.str();
		}
        
		LogEntry& operator=(const LogEntry&) = delete;
		LogEntry& operator=(LogEntry&&) = delete;

		~LogEntry() {
			if (!disabled) {
				//MUST NOT throw !
				callback(*this);
				disabled = true;
			}
		}

		const loggerdetail::LogContext& getContext() const {
			return context;
		}

        std::string getMessage() const {
            return fullMessage.str();
        }

        const loggerdetail::LogTimePoint& getDate() const {
            return date;
        }

		void disable(){
			callback = LogCallback {};
			disabled = true;
		}

		void nextPatternReccursionLevel() {
			patternReccursionLevel++;
		}

		bool isPatternReccursionFirstLevel() const {
			return patternReccursionLevel == 0;
		}

    private:
		loggerdetail::LogContext context;
        //Mutable used safely because LogEntry is only a short time wrapper-class that is destroyed at the end of the log line
        mutable std::stringstream fullMessage;
        loggerdetail::LogTimePoint date;
		LogCallback callback;
        bool disabled;
				std::size_t patternReccursionLevel = 0;

        static loggerdetail::LogTimePoint currentDateTime();
        
        template <class T>
        friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);

		template <LogLevel MinLevel, LogLevel MaxLevel, class LogMethod>
		friend class Logger;
    };
    
    template <class T>
    const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart) {
		if (logEntry.disabled) {
			return logEntry;
		}

        logEntry.fullMessage << std::forward<T>(logPart);
        return logEntry;
    }
    
}

