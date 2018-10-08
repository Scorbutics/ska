#pragma once
#include <string>
#include <string>
#include <unordered_map>

#include "Tokenizer.h"
#include "MessagesBuilder.h"

namespace ska {

	enum class LogLevel {
		Debug = 0,
		Info = 1,
		Warn = 2,
		Error = 3,
		Disabled = 100
	};

	template<class T>
	class LoggerClassFormatter;
	
	template<class T>
	class LoggerClassLevel {
    public:
        static constexpr const auto level = LogLevel::Debug;
    };

	namespace loggerdetail {
	    class LogEntry;
        class Logger {
            friend class LogEntry;
        private:
            Logger(const Logger&) = delete;

            void applyTokenOnOutput(const tm& date, const loggerdetail::Token& token, const std::string& messages);
            void consumeTokens(const tm& date, const std::string& messages);
            
        protected:
            Logger(std::string className);

            Logger(std::string className, std::ostream& output) :
                m_logLevel(LogLevel::Debug),
                m_className(std::move(className)),
                m_output(output) {
                
                const auto& defaultPattern = "%10c[%h:%m:%s] %14c%C %15c%v";
                m_pattern.emplace(LogLevel::Debug, loggerdetail::Tokenizer { defaultPattern });
                m_pattern.emplace(LogLevel::Info, loggerdetail::Tokenizer { defaultPattern });
                m_pattern.emplace(LogLevel::Error, loggerdetail::Tokenizer { defaultPattern });
                m_pattern.emplace(LogLevel::Warn, loggerdetail::Tokenizer { defaultPattern });
            }
        
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

            virtual ~Logger() = default;

        private:
            LogLevel m_logLevel;
            const std::string m_className;
            std::ostream& m_output;
            std::unordered_map<LogLevel, loggerdetail::Tokenizer> m_pattern;
            
            template <class T>
			friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);
        };
        
        class LogEntry {
		public:
            LogEntry(Logger& instance, LogLevel logLevel) : instance(&instance), logLevel(logLevel), date(currentDateTime())  {}
			LogEntry(){}
            ~LogEntry() {
				//MUST NOT throw !
				if(instance != nullptr) {
                    instance->consumeTokens(date, message.str());
                }
			}
        private:
			Logger* instance = nullptr;
			LogLevel logLevel;
			
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
            if (logEntry.logLevel >= logEntry.instance->m_logLevel) {
                logEntry.message << std::forward<T>(logPart);
            }
            return logEntry;
        }

        struct EmptyProxy {};
        template <class T>
        const EmptyProxy& operator<<(const EmptyProxy& logEntry, T&& logPart) {
            return logEntry;
        }
    }
    
	template <class Wrapped>
    class Logger : public loggerdetail::Logger {
    public:
        Logger(std::ostream& output) :
            loggerdetail::Logger(std::move(LoggerClassFormatter<Wrapped>::format()), output) {
        }
        
        Logger() :
            loggerdetail::Logger(std::move(LoggerClassFormatter<Wrapped>::format())) {}

        template<LogLevel logLevel>
        auto log() {
            if constexpr (logLevel >= LoggerClassLevel<Wrapped>::level) {
                return loggerdetail::LogEntry{ *this, logLevel };
            } else {
                return loggerdetail::EmptyProxy{};
            }   
        }

        ~Logger() = default;
    };
}

/*
#define SKA_DEFAULT_OUTPUT std::cout
#define SKA_LOG_COMMON_PART_DEF_ACCESS ska::LoggerFactory::access(this, SKA_DEFAULT_OUTPUT)
#define SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass) ska::LoggerFactory::access<loggerClass>(SKA_DEFAULT_OUTPUT)


#if defined(NDEBUG) && !defined(SKA_LOG_FORCE_ACTIVATE)
#define SKA_LOG_DEBUG true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.debug
#define SKA_LOG_INFO true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.info
#define SKA_LOG_WARN true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.warn
#define SKA_LOG_ERROR true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.error

#define SKA_STATIC_LOG_DEBUG(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).debug
#define SKA_STATIC_LOG_INFO(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).info
#define SKA_STATIC_LOG_WARN(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).warn
#define SKA_STATIC_LOG_ERROR(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).error
#else
#define SKA_LOG_DEBUG SKA_LOG_COMMON_PART_DEF_ACCESS.debug
#define SKA_LOG_INFO SKA_LOG_COMMON_PART_DEF_ACCESS.info
#define SKA_LOG_WARN SKA_LOG_COMMON_PART_DEF_ACCESS.warn
#define SKA_LOG_ERROR SKA_LOG_COMMON_PART_DEF_ACCESS.error

#define SKA_STATIC_LOG_DEBUG(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).debug
#define SKA_STATIC_LOG_INFO(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).info
#define SKA_STATIC_LOG_WARN(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).warn
#define SKA_STATIC_LOG_ERROR(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).error
#endif
*/
