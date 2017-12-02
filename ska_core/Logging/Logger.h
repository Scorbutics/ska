#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>

namespace ska {
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

	enum EnumLogLevel {
		DEBUG = 0,
    	INFO = 1,
		MESSAGE = 2,
		ERROR = 3
	};

	class Logger {
	private:
		Logger() = default;

		class LoggerInstance {
		public:
			LoggerInstance() :
				logLevel(EnumLogLevel::DEBUG) {
			}

			template<class ...T>
			void debug(const T&... message) {
				if (logLevel <= EnumLogLevel::DEBUG) {
					printDateTime(std::cout);
					loggerdetail::LoggerImpl<T...>::debug(std::forward<const T&>(message)...);
				}
			}

			template<class ...T>
			void info(const T&... message) {
				if (logLevel <= EnumLogLevel::INFO) {
					printDateTime(std::cout);
					loggerdetail::LoggerImpl<T...>::info(std::forward<const T&>(message)...);
				}
			}

			template<class ...T>
			void log(const T&... message) {
				if (logLevel <= EnumLogLevel::MESSAGE) {
					printDateTime(std::cout);
					loggerdetail::LoggerImpl<T...>::log(std::forward<const T&>(message)...);
				}
			}

			template<class ...T>
			void error(const T&... message) {
				if (logLevel <= EnumLogLevel::ERROR) {
					printDateTime(std::cerr);
					loggerdetail::LoggerImpl<T...>::error(std::forward<const T&>(message)...);
				}
			}
		
			EnumLogLevel logLevel;
		};

	public:
		~Logger() = default;

		static void configureLogLevel(EnumLogLevel logLevel) {
			getInstance().logLevel = logLevel;
		}

		template<class ...T>
		static void log(const T&... message) {
			getInstance().log(std::forward<const T&>(message)...);
		}

		template<class ...T>
		static void info(const T&... message) {
			getInstance().info(std::forward<const T&>(message)...);
		}

		template<class ...T>
		static void debug(const T&... message) {
			getInstance().debug(std::forward<const T&>(message)...);
		}

		template<class ...T>
		static void error(const T&... message) {
			getInstance().error(std::forward<const T&>(message)...);
		}

	private:
		static LoggerInstance& getInstance() {
			static LoggerInstance instance;
			return instance;
		}

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

}

#ifdef NDEBUG
#define SKA_LOG_DEBUG true ? (void)0 : ska::Logger::debug
#define SKA_LOG_INFO true ? (void)0 : ska::Logger::info
#define SKA_LOG_MESSAGE true ? (void)0 : ska::Logger::log
#define SKA_LOG_ERROR true ? (void)0 : ska::Logger::error
#else
#define SKA_LOG_DEBUG ska::Logger::debug
#define SKA_LOG_INFO ska::Logger::info
#define SKA_LOG_MESSAGE ska::Logger::log
#define SKA_LOG_ERROR ska::Logger::error
#endif
