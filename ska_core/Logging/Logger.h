#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include "../Utils/NonCopyable.h"

namespace ska {

	enum EnumLogLevel {		
		DEBUG = 0,
		INFO = 1,
		MESSAGE = 2,
		ERROR = 3,
		DISABLED = 100
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
				std::cout << std::endl;
			}

			static void info() {
				std::cout << std::endl;
			}

			static void error() {
				std::cerr << std::endl;
			}

			static void debug() {
				std::cout << std::endl;
			}
		};

	}

	class Logger : public LoggerLogLevel, public NonCopyable {
		friend class LoggerFactory;
	private:
		Logger() :
			m_logLevel(EnumLogLevel::DEBUG) {
		}

	public:
		template<class ...T>
		void debug(const T&... message) {
			if (m_logLevel <= EnumLogLevel::DEBUG) {
				printDateTime(std::cout);
				loggerdetail::LoggerImpl<T...>::debug(std::forward<const T&>(message)...);
			}
		}

		template<class ...T>
		void info(const T&... message) {
			if (m_logLevel <= EnumLogLevel::INFO) {
				printDateTime(std::cout);
				loggerdetail::LoggerImpl<T...>::info(std::forward<const T&>(message)...);
			}
		}

		template<class ...T>
		void log(const T&... message) {
			if (m_logLevel <= EnumLogLevel::MESSAGE) {
				printDateTime(std::cout);
				loggerdetail::LoggerImpl<T...>::log(std::forward<const T&>(message)...);
			}
		}

		template<class ...T>
		void error(const T&... message) {
			if (m_logLevel <= EnumLogLevel::ERROR) {
				printDateTime(std::cerr);
				loggerdetail::LoggerImpl<T...>::error(std::forward<const T&>(message)...);
			}
		}

		void configureLogLevel(EnumLogLevel logLevel) override {
			m_logLevel = logLevel;
		}


		~Logger() = default;

	private:
		EnumLogLevel m_logLevel;

		static void printDateTime(std::ostream& os) {
			auto t = std::time(nullptr);
#ifdef _MSC_VER
			struct tm buf;
			localtime_s(&buf, &t);
#else
			struct tm buf = *std::localtime(&t);
#endif
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
			static Logger logger;
			return logger;
		}

		template <class T>
		static Logger& access(T* dummy){
			return staticAccess<T>();
		}
	};
	

}

#if defined(NDEBUG) && !defined(SKA_LOG_FORCE_ACTIVATE)
#define SKA_LOG_DEBUG true ? (void)0 : ska::LoggerFactory::access(this).debug
#define SKA_LOG_INFO true ? (void)0 : ska::LoggerFactory::access(this).info
#define SKA_LOG_MESSAGE true ? (void)0 : ska::LoggerFactory::access(this).log
#define SKA_LOG_ERROR true ? (void)0 : ska::LoggerFactory::access(this).error
#else
#define SKA_LOG_DEBUG ska::LoggerFactory::access(this).debug
#define SKA_LOG_INFO ska::LoggerFactory::access(this).info
#define SKA_LOG_MESSAGE ska::LoggerFactory::access(this).log
#define SKA_LOG_ERROR ska::LoggerFactory::access(this).error

#define SKA_STATIC_LOG_DEBUG(loggerClass) ska::LoggerFactory::staticAccess<loggerClass>().debug
#define SKA_STATIC_LOG_INFO(loggerClass) ska::LoggerFactory::staticAccess<loggerClass>().info
#define SKA_STATIC_LOG_MESSAGE(loggerClass) ska::LoggerFactory::staticAccess<loggerClass>().log
#define SKA_STATIC_LOG_ERROR(loggerClass) ska::LoggerFactory::staticAccess<loggerClass>().error
#endif
