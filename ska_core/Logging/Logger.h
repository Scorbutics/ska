#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <ctime>
#include <typeinfo>
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "ColorStream.h"

#include "Tokenizer.h"
#include "MessagesBuilder.h"

namespace ska {

	enum class EnumLogLevel {
		SKA_DEBUG = 0,
		SKA_INFO = 1,
		SKA_WARN = 2,
		SKA_ERROR = 3,
		SKA_DISABLED = 100
	};

	template<class T>
    class LoggerClassFormatter;
	
	class Logger {
	private:
		Logger(const Logger&) = delete;
		Logger(Logger&&) = default;
		
		Logger(std::string className, std::ostream& output) :
			m_logLevel(EnumLogLevel::SKA_DEBUG),
			m_className(std::move(className)),
            m_output(output) {
			
			const auto& defaultPattern = "%10c[%h:%m:%s] %14c%C %15c%v";
			m_pattern.emplace(EnumLogLevel::SKA_DEBUG, loggerdetail::Tokenizer { defaultPattern });
			m_pattern.emplace(EnumLogLevel::SKA_INFO, loggerdetail::Tokenizer { defaultPattern });
			m_pattern.emplace(EnumLogLevel::SKA_ERROR, loggerdetail::Tokenizer { defaultPattern });
			m_pattern.emplace(EnumLogLevel::SKA_WARN, loggerdetail::Tokenizer { defaultPattern });
		}

        void applyTokenOnOutput(const tm& date, const loggerdetail::Token& token, const std::vector<std::string>& messages);
        void consumeTokens(const tm& date, const std::vector<std::string>& messages);
        
        template <class ...Args>
        void commonLog(const Args& ... message) {
            auto messages = std::vector<std::string>(sizeof ... (Args));
            loggerdetail::MessagesBuilder<Args...>::buildMessages(messages, std::forward<const Args&>(message)...);
            const auto date = currentDateTime();
            consumeTokens(date, messages);
        }

	public:
		template <class T>
		static Logger build(std::ostream& output) {
			return Logger(LoggerClassFormatter<T>::format(), output);
		}
	
        template<class ...Args>
		void debug(const Args&... message) {
            if (m_logLevel <= EnumLogLevel::SKA_DEBUG) {
                commonLog(std::forward<const Args&>(message)...);
			}
		}

		template<class ...Args>
		void info(const Args&... message) {
            if (m_logLevel <= EnumLogLevel::SKA_INFO) {
				commonLog(std::forward<const Args&>(message)...);
			}
		}

		template<class ...Args>
		void warn(const Args&... message) {
            if (m_logLevel <= EnumLogLevel::SKA_WARN) {
				commonLog(std::forward<const Args&>(message)...);
			}
		}

		template<class ...Args>
		void error(const Args&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_ERROR) {
				commonLog(std::forward<const Args&>(message)...);
			}
		}

		void configureLogLevel(EnumLogLevel logLevel) {
			m_logLevel = logLevel;
		}
        
        void setPattern(EnumLogLevel logLevel, std::string pattern) {
            auto existingLoglevel = m_pattern.find(logLevel);
			if(existingLoglevel != m_pattern.end()) {
				m_pattern.erase(existingLoglevel);
			}
			m_pattern.emplace(logLevel, loggerdetail::Tokenizer {std::move(pattern)});
        }

		~Logger() = default;

	private:
		EnumLogLevel m_logLevel;
		const std::string m_className;
        std::ostream& m_output;
        std::unordered_map<EnumLogLevel, loggerdetail::Tokenizer> m_pattern;

		static tm currentDateTime();
	};

    class LoggerFactory {
	private:
		LoggerFactory() = default;
		static unsigned int m_classNameMaxLength;
		
	public:
		~LoggerFactory() = default;

		template <class T>
		static Logger& access(std::ostream& output) {
			static auto logger = Logger::build<T>(output);
			return logger;
		}

		template <class T>
		static Logger& access(T*, std::ostream& output) {
			return access<T>(output);
		}
		
	};

}


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

