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
        };
    }

	class Logger {
	private:
		Logger() = default;

	public:
		~Logger() = default;

		template<class ...T>
		static void log(const T&... message) {
			printDateTime(std::cout);
			loggerdetail::LoggerImpl<T...>::log(std::forward<const T&>(message)...);
		}

		template<class ...T>
		static void info(const T&... message) {
			printDateTime(std::cout);
			loggerdetail::LoggerImpl<T...>::info(std::forward<const T&>(message)...);
		}

		template<class ...T>
		static void error(const T&... message) {
			printDateTime(std::cerr);
			loggerdetail::LoggerImpl<T...>::error(std::forward<const T&>(message)...);
		}

	private:
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
#define SKA_LOG_INFO true ? (void)0 : ska::Logger::info
#define SKA_LOG_MESSAGE true ? (void)0 : ska::Logger::log
#define SKA_LOG_ERROR true ? (void)0 : ska::Logger::error
#else
#define SKA_LOG_INFO ska::Logger::info
#define SKA_LOG_MESSAGE ska::Logger::log
#define SKA_LOG_ERROR ska::Logger::error
#endif
