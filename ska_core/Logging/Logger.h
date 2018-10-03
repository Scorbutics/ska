#pragma once
#include <memory>
#include <string>
#include <ostream>
#include <ctime>
#include <typeinfo>
#include <cassert>
#include <iomanip>
#include "../Utils/MovableNonCopyable.h"
#include "ColorStream.h"

namespace ska {

	enum class EnumLogLevel {
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
			static void log(std::ostream& output, const T1& message, const T&... remainingMessages) {
				output << message;
				LoggerImpl<T...>::log(std::forward<const T&>(remainingMessages)...);
			}
		};

		template<>
		struct LoggerImpl<> {
			static void log(std::ostream& output) {
				output << '\n';
			}
		};

        enum class TokenType {
            Color,
            Message,
            Year,
            Month,
            Day,
            Hour,
            Minute,
            Second,
            Literal,
            Empty
        };

        struct Token {
            std::string value;
            TokenType type = TokenType::Empty;
        };

        class Tokenizer {
        public:
            Tokenizer(const std::string& str);
            ~Tokenizer() = default;
            const Token& next() const {
                return m_cursor < m_tokens.size() ? m_tokens[m_cursor++] : m_emptyToken;
            }

        private:
            static std::vector<Token> parse(const std::string& str);
            std::vector<Token> m_tokens;
            std::size_t m_cursor = 0;
            const Token m_emptyToken;
        };

	}

	class Logger : public LoggerLogLevel, public MovableNonCopyable {
		friend class LoggerFactory;
	private:
		/*
         static std::string prettifyClassName(const std::string& cs, unsigned int maxLength){
			static const std::string classKeyword = "class ";
			const auto paddedLength = maxLength;

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
        */

		Logger(const std::string& className, std::ostream& output) :
			m_logLevel(EnumLogLevel::SKA_DEBUG),
			m_className(className),
            m_output(output) {
		}

	public:
		template<class ...T>
		void debug(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_DEBUG) {
				printDateTime(m_output);
				m_output << EnumColorStream::CYAN;
				loggerdetail::LoggerImpl<const std::string&, T...>::debug(m_className, std::forward<const T&>(message)...);
				m_output << EnumColorStream::WHITE;
			}
		}

		template<class ...T>
		void info(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_INFO) {
				printDateTime(m_output);
				m_output << EnumColorStream::LIGHTGREEN;
				loggerdetail::LoggerImpl<const std::string&, T...>::info(m_className, std::forward<const T&>(message)...);
				m_output << EnumColorStream::WHITE;
			}
		}

		template<class ...T>
		void log(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_MESSAGE) {
				printDateTime(m_output);
				m_output << EnumColorStream::YELLOW;
				loggerdetail::LoggerImpl<const std::string&, T...>::log(m_className, std::forward<const T&>(message)...);
				m_output << EnumColorStream::WHITE;
			}
		}

		template<class ...T>
		void error(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_ERROR) {
				printDateTime(m_output);
				m_output << EnumColorStream::LIGHTRED;
				loggerdetail::LoggerImpl<const std::string&, T...>::error(m_className, std::forward<const T&>(message)...);
				m_output << EnumColorStream::WHITE;
			}
		}

		void configureLogLevel(EnumLogLevel logLevel) override {
			m_logLevel = logLevel;
		}
        
        void setPattern(std::string pattern) {
            m_pattern = pattern;
        }

		~Logger() = default;

	private:
		EnumLogLevel m_logLevel;
		const std::string m_className;
        std::ostream& m_output;
        std::string m_pattern;

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

    template<class T>
    class LoggerClassFormatter;

    class LoggerFactory {
	private:
		LoggerFactory() = default;
		static unsigned int m_classNameMaxLength;
		
	public:
		~LoggerFactory() = default;

		template <class T>
		static Logger& access() {
			static Logger logger(LoggerClassFormatter<T>::format());
			return logger;
		}

		template <class T>
		static Logger& access(T*) {
			return access<T>();
		}
		
	};

}

#define SKA_LOG_COMMON_PART_DEF_ACCESS ska::LoggerFactory::access(this)
#define SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass) ska::LoggerFactory::access<loggerClass>()


#if defined(NDEBUG) && !defined(SKA_LOG_FORCE_ACTIVATE)
#define SKA_LOG_DEBUG true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.debug
#define SKA_LOG_INFO true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.info
#define SKA_LOG_MESSAGE true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.log
#define SKA_LOG_ERROR true ? (void)0 : SKA_LOG_COMMON_PART_DEF_ACCESS.error

#define SKA_STATIC_LOG_DEBUG(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).debug
#define SKA_STATIC_LOG_INFO(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).info
#define SKA_STATIC_LOG_MESSAGE(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).log
#define SKA_STATIC_LOG_ERROR(loggerClass) true ? (void)0 : SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).error
#else
#define SKA_LOG_DEBUG SKA_LOG_COMMON_PART_DEF_ACCESS.debug
#define SKA_LOG_INFO SKA_LOG_COMMON_PART_DEF_ACCESS.info
#define SKA_LOG_MESSAGE SKA_LOG_COMMON_PART_DEF_ACCESS.log
#define SKA_LOG_ERROR SKA_LOG_COMMON_PART_DEF_ACCESS.error

#define SKA_STATIC_LOG_DEBUG(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).debug
#define SKA_STATIC_LOG_INFO(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).info
#define SKA_STATIC_LOG_MESSAGE(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).log
#define SKA_STATIC_LOG_ERROR(loggerClass) SKA_STATIC_LOG_COMMON_PART_DEF_(loggerClass).error
#endif
