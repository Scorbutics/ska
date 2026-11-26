#pragma once

#include "LogEntry.h"
#include "LoggerImpl.h"
#include "LogLevel.h"
#include "LogContext.h"
#include "LogSync.h"

namespace ska {

	template<class T>
	class LoggerClassFormatter {
	public:
		static constexpr const auto className = "";
	};
	
	template<class T>
	class LoggerClassLevel {
    public:
        static constexpr const auto level = LogLevel::Debug;
    };

    namespace loggerdetail {
        struct EmptyProxy {};
        template <class T>
        const EmptyProxy& operator<<(const EmptyProxy& logEntry, T&&) {
            return logEntry;
        }
    }
    
	template <LogLevel MinLevel = LogLevel::Debug, LogLevel MaxLevel = LogLevel::Error, class LogMethod = LogSync>
    class Logger : public loggerdetail::Logger {
		template <class ... LoggerC>
		friend class MultiLogger;
	
	static_assert(MinLevel <= MaxLevel, "Max log level must be greater or equal than Min log level");

	private:
		template<LogLevel logLevel, unsigned long line>
		LogEntry log(const char* className, bool disabled, const char* functionName, const char* filename) {
			return LogEntry{ [this](LogEntry& self) {
				m_logMethod.log(self, *this);
			}, loggerdetail::LogContext { logLevel, className, functionName, filename, line }, disabled };
		}
			
		void onDestroyEntry(LogEntry& self) {
			m_logMethod.log(self, *this);
		}

	public:
		template <LogLevel logLevel>
		static constexpr bool accept() {
			return (logLevel >= MinLevel && logLevel <= MaxLevel);
		}
	
		Logger() = default;
		
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger& operator=(Logger&&) = delete;

		Logger(Logger&&) = default;

        Logger(std::ostream& output, LogFilter filter) {
		addOutputTarget(output, std::move(filter));
        }
		
		template <LogLevel logLevel, class Wrapped, unsigned long line = 0l>
		auto log(const char* functionName = "", const char* filename = "") {
			static_assert(logLevel != LogLevel::Disabled, "Unable to log at log level \"Disabled\"");
			if constexpr (logLevel >= LoggerClassLevel<Wrapped>::level &&
				(logLevel >= MinLevel && logLevel <= MaxLevel)) {
				return log<logLevel, line>(LoggerClassFormatter<Wrapped>::className, logLevel < getLogLevel(), functionName, filename);
			} else {
				(void) functionName;
				(void) filename;
				return loggerdetail::EmptyProxy{};
			}
		}

		void terminate() {
			m_logMethod.terminate();
		}
		
        ~Logger() = default;
    
    private:
        LogMethod m_logMethod;
    };
}

#ifndef SKA_LOG_DONT_USE_LOG_DEFINE

	#ifdef __PRETTY_FUNCTION__
	#define SKA_CURRENT_FUNCTION __PRETTY_FUNCTION__
	#elif defined __FUNCSIG__
	#define SKA_CURRENT_FUNCTION __FUNCSIG__ 
	#else
	#define SKA_CURRENT_FUNCTION __func__
	#endif

	#define SKA_LOGC_STATIC(logger, level, currentClass) logger.log<level, currentClass, __LINE__>(SKA_CURRENT_FUNCTION, __FILE__ )
	#define SKA_LOGC(logger, level) SKA_LOGC_STATIC(logger, level, typename std::decay<decltype(*this)>::type)
	#define SKA_LOGC_CONFIG(logLevel, currentClass) namespace ska { \
		template <> \
		class LoggerClassLevel<currentClass> { \
		public: \
			static constexpr const auto level = logLevel; \
		}; \
	}

#endif
