#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include "../Utils/NonCopyable.h"
#include "ColorStream.h"

namespace ska {

	enum EnumLogLevel {
		SKA_DEBUG = 0,
		SKA_INFO = 1,
		SKA_MESSAGE = 2,
		SKA_ERROR = 3,
		SKA_DISABLED = 100
	};

	class LoggerLogLevel {
	public:
		LoggerLogLevel() = default;
		virtual void configureLogLevel(EnumLogLevel logLevel) = 0;
		virtual ~LoggerLogLevel() = default;
	};

	namespace loggerdetail {
		template<class ...Args>
		struct LoggerImpl;

		template<class T1, class ...T>
		struct LoggerImpl<T1, T...> {
			static void log(const T1& message, const T&... remainingMessages) {
				std::cout << message;
				LoggerImpl<T...>::log(std::forward<const T&>(remainingMessages)...);
			}

			static void error(const T1& message, const T&... remainingMessages) {
				std::cerr << message;
				LoggerImpl<T...>::error(std::forward<const T&>(remainingMessages)...);
			}

			static void info(const T1& message, const T&... remainingMessages) {
				std::cout << message;
				LoggerImpl<T...>::info(std::forward<const T&>(remainingMessages)...);
			}

			static void debug(const T1& message, const T&... remainingMessages) {
				std::cout << message;
				LoggerImpl<T...>::log(std::forward<const T&>(remainingMessages)...);
			}

		};

		template<>
		struct LoggerImpl<> {
			static void log() {
				std::cout << '\n';
			}

			static void info() {
				std::cout << '\n';
			}

			static void error() {
				std::cerr << std::endl;
			}

			static void debug() {
				std::cout << '\n';
			}
		};

	}

	class Logger : public LoggerLogLevel, public NonCopyable {
		friend class LoggerFactory;
	private:
		static std::string prettifyClassName(const std::string& cs){
			static const std::string classKeyword = "class ";
			static const auto paddedLength = 20;
			
			auto formattedCs = cs;
			if(classKeyword == cs.substr(0, classKeyword.size())) {
				formattedCs = cs.substr(classKeyword.size());
			}
			formattedCs = formattedCs.substr(0, paddedLength);
			if (paddedLength - formattedCs.size() > 0) {
				formattedCs += std::string(paddedLength - formattedCs.size(), ' ');
			}
			return "(" + formattedCs + ") ";
		}

		explicit Logger(const std::string& className) :
			m_logLevel(EnumLogLevel::SKA_DEBUG),
			m_className(prettifyClassName(className)) {
		}

	public:
		template<class ...T>
		void debug(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_DEBUG) {
				printDateTime(std::cout);
				std::cout << EnumColorStream::LIGHTGREEN;
				loggerdetail::LoggerImpl<const std::string&, T...>::debug(m_className, std::forward<const T&>(message)...);
				std::cout << EnumColorStream::WHITE;
			}
		}

		template<class ...T>
		void info(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_INFO) {
				printDateTime(std::cout);
				std::cout << EnumColorStream::CYAN;
				loggerdetail::LoggerImpl<const std::string&, T...>::info(m_className, std::forward<const T&>(message)...);
				std::cout << EnumColorStream::WHITE;
			}
		}

		template<class ...T>
		void log(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_MESSAGE) {
				printDateTime(std::cout);
				std::cout << EnumColorStream::YELLOW;
				loggerdetail::LoggerImpl<const std::string&, T...>::log(m_className, std::forward<const T&>(message)...);
				std::cout << EnumColorStream::WHITE;
			}
		}

		template<class ...T>
		void error(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_ERROR) {
				printDateTime(std::cerr);
				std::cerr << EnumColorStream::RED;
				loggerdetail::LoggerImpl<const std::string&, T...>::error(m_className, std::forward<const T&>(message)...);
				std::cerr << EnumColorStream::WHITE;
			}
		}

		void configureLogLevel(EnumLogLevel logLevel) override {
			m_logLevel = logLevel;
		}


		~Logger() = default;

	private:
		EnumLogLevel m_logLevel;
		const std::string m_className;

		static void printDateTime(std::ostream& os) {
			auto t = std::time(nullptr);
#ifdef _MSC_VER
			struct tm buf;
			localtime_s(&buf, &t);
#else
			struct tm buf = *std::localtime(&t);
#endif
			std::cout << EnumColorStream::LIGHTMAGENTA;
			os << "[" << std::put_time(&buf, "%H:%M:%S") << "] ";
		}
	};

	class LoggerFactory {
	private:
		LoggerFactory() = default;

	public:
		~LoggerFactory() = default;

		template <class T>
		static Logger& staticAccess() {
			return staticAccess<T>(typeid(T).name());
		}

		template <class T>
		static Logger& staticAccess(const std::string& className) {
			static Logger logger(className);
			return logger;
		}

		template <class T>
		static Logger& access(const std::string& className, T* dummy) {
			return staticAccess<T>(className);
		}
	};
	
}

#define SKA_REGISTER_PARSE_TYPE(X) #X 
#define SKA_LOG_COMMON_PART_DEF_RTTI ska::LoggerFactory::access(typeid(*this).name(), this)
#define SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass) ska::LoggerFactory::staticAccess<loggerClass>(SKA_REGISTER_PARSE_TYPE(loggerClass))


#if defined(NDEBUG) && !defined(SKA_LOG_FORCE_ACTIVATE)
#define SKA_LOG_DEBUG true ? (void)0 : SKA_LOG_COMMON_PART_DEF_RTTI.debug
#define SKA_LOG_INFO true ? (void)0 : SKA_LOG_COMMON_PART_DEF_RTTI.info
#define SKA_LOG_MESSAGE true ? (void)0 : SKA_LOG_COMMON_PART_DEF_RTTI.log
#define SKA_LOG_ERROR true ? (void)0 : SKA_LOG_COMMON_PART_DEF_RTTI.error

#define SKA_STATIC_LOG_DEBUG(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).debug
#define SKA_STATIC_LOG_INFO(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).info
#define SKA_STATIC_LOG_MESSAGE(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).log
#define SKA_STATIC_LOG_ERROR(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).error
#else
#define SKA_LOG_DEBUG SKA_LOG_COMMON_PART_DEF_RTTI.debug
#define SKA_LOG_INFO SKA_LOG_COMMON_PART_DEF_RTTI.info
#define SKA_LOG_MESSAGE SKA_LOG_COMMON_PART_DEF_RTTI.log
#define SKA_LOG_ERROR SKA_LOG_COMMON_PART_DEF_RTTI.error

#define SKA_STATIC_LOG_DEBUG(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).debug
#define SKA_STATIC_LOG_INFO(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).info
#define SKA_STATIC_LOG_MESSAGE(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).log
#define SKA_STATIC_LOG_ERROR(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).error
#endif
