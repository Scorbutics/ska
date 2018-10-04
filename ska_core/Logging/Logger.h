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

#include "../Utils/MovableNonCopyable.h"
#include "ColorStream.h"

namespace ska {

	enum class EnumLogLevel {
		SKA_DEBUG = 0,
		SKA_INFO = 1,
		SKA_WARN = 2,
		SKA_ERROR = 3,
		SKA_DISABLED = 100
	};

	namespace loggerdetail {

        enum class TokenType {
            Color,
			Class,
            Value,
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
        public:
			Token() = default;
            Token(std::string value, TokenType type, std::size_t length = 0) :
				m_value(std::move(value)),
				m_type(type), 
				m_length(length) {
				if(m_length == 0) {
					m_length = m_value.size();
				}
			}
		
            std::size_t length() const {
                return m_length;
            }

            TokenType type() const {
                return m_type;
            }
            
            const std::string& value() const {
                return m_value;
            }

		private:
			std::string m_value;
            TokenType m_type = TokenType::Empty;
			std::size_t m_length = 0;
        };

        class Tokenizer {
        public:
            Tokenizer(const std::string& str);
            ~Tokenizer() = default;
            
            const Token& next() {
                return m_cursor < m_tokens.size() ? m_tokens[m_cursor++] : m_emptyToken;
            }

            bool empty() const {
                return m_cursor >= m_tokens.size();
            }

            void rewind() {
                m_cursor = 0;
            }

        private:
            static std::vector<Token> parse(const std::string& str);
            
            static Token parseLiteral(const std::string& str, std::size_t& index);
			static Token parsePlaceholder(const std::string& str, std::size_t& index);
			
            static std::size_t matchOptionalNumeric(const std::string& str, std::size_t& index);
            static std::pair<std::string, TokenType> matchCharacterNonNumeric(const std::string& str, std::size_t& index);

			std::vector<Token> m_tokens;
            std::size_t m_cursor = 0;
            Token m_emptyToken;
        };

        template <class ...T>
        struct MessagesBuilder;

        template <>
        struct MessagesBuilder<> {
            static void buildMessages(std::vector<std::string>& messagesOutput) {}
        };

        template <class T1, class ...T>
        struct MessagesBuilder<T1, T...> {
            static void buildMessages(std::vector<std::string>& messagesOutput, const T1& message, const T& ... messageList) {
                {
                    auto ss = std::stringstream {};
                    ss << message;
                    messagesOutput.push_back(ss.str());
                }
                MessagesBuilder<T...>::buildMessages(messagesOutput, std::forward<const T&>(messageList) ...);
            }
        };
	}

	class Logger : public MovableNonCopyable {
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
			
			const auto& defaultPattern = "%10c[%h:%m:%s] %14c%C %15c%v";
			m_pattern.emplace(EnumLogLevel::SKA_DEBUG, loggerdetail::Tokenizer { defaultPattern });
			m_pattern.emplace(EnumLogLevel::SKA_INFO, loggerdetail::Tokenizer { defaultPattern });
			m_pattern.emplace(EnumLogLevel::SKA_ERROR, loggerdetail::Tokenizer { defaultPattern });
			m_pattern.emplace(EnumLogLevel::SKA_WARN, loggerdetail::Tokenizer { defaultPattern });
		}


        void applyTokenOnOutput(const tm& date, const loggerdetail::Token& token, const std::vector<std::string>& messages);
        void consumeTokens(const tm& date, const std::vector<std::string>& messages);
        
        template <class ...T>
        void commonLog(const T& ... message) {
            auto messages = std::vector<std::string>( sizeof ... (T));
            loggerdetail::MessagesBuilder<T...>::buildMessages(messages, std::forward<const T&>(message)...);
            const auto date = printDateTime(m_output);
            consumeTokens(date, messages);
        }

	public:
        template<class ...T>
		void debug(const T&... message) {
            if (m_logLevel <= EnumLogLevel::SKA_DEBUG) {
                commonLog(std::forward<const T&>(message)...);
			}
		}

		template<class ...T>
		void info(const T&... message) {
            if (m_logLevel <= EnumLogLevel::SKA_INFO) {
				commonLog(std::forward<const T&>(message)...);
			}
		}

		template<class ...T>
		void warn(const T&... message) {
            if (m_logLevel <= EnumLogLevel::SKA_WARN) {
				commonLog(std::forward<const T&>(message)...);
			}
		}

		template<class ...T>
		void error(const T&... message) {
			if (m_logLevel <= EnumLogLevel::SKA_ERROR) {
				commonLog(std::forward<const T&>(message)...);
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

		static tm printDateTime(std::ostream& os);
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
		static Logger& access(std::ostream& output) {
			static Logger logger(LoggerClassFormatter<T>::format(), output);
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
