#pragma once

#include <sstream>
#include <cassert>
#include "LogLevel.h"
#include "Tokenizer.h"
#include "LoggerImpl.h"

namespace ska {

    class LogEntry {
    protected:
        LogEntry(loggerdetail::Logger& instance, LogLevel logLevel, std::size_t id) : 
            id(id),
            instance(&instance), 
            logLevel(logLevel), 
            date(currentDateTime()),
            className(instance.m_className) {
        }

    public:
        LogEntry(const LogEntry&) = delete;
        LogEntry& operator=(const LogEntry&) = delete;

        LogEntry(LogEntry&&) = default;
        LogEntry& operator=(LogEntry&&) = default;

        ~LogEntry() = default;

        const std::string& getClassName() const {
            return className;
        }

        const LogLevel& getLogLevel() const {
            return logLevel;
        }

        std::string getMessage() const {
            return fullMessage.str();
        }

        const tm& getDate() const {
            return date;
        }

        const std::size_t getId() const {
            return id;
        }

        void consumeTokens();

    private:
        std::size_t id;
        bool alreadyLogged = false;
        loggerdetail::Logger* instance;
        LogLevel logLevel;
        //Mutable used because LogEntry is only a short time wrapper-class that is destroyed at the end of the log line
        mutable std::stringstream fullMessage;
        tm date;
        std::string className;
        
        static tm currentDateTime();
        
        template <class T>
        friend const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart);	
    };
    
    template <class T>
    const LogEntry& operator<<(const LogEntry& logEntry, T&& logPart) {
        if (logEntry.logLevel >= logEntry.instance->m_logLevel) {
            logEntry.fullMessage << std::forward<T>(logPart);
        }
        return logEntry;
    }

    //templated version containing the log method
    namespace loggerdetail {
        template <class LogMethod>
        class LogEntry : public ska::LogEntry {
        private:
            LogMethod* m_logMethod;
            static std::size_t InstanceCounter;
        public:
            LogEntry(Logger& instance, LogLevel logLevel, LogMethod& method) : 
                ska::LogEntry(instance, logLevel, InstanceCounter++),
                m_logMethod(&method) {
            }
            
            LogEntry(const LogEntry&) = delete;
            LogEntry& operator=(const LogEntry&) = delete;

			LogEntry(LogEntry&& e) {
				*this = std::move(e);
			}
			
			LogEntry& operator=(LogEntry&& e) {
				m_logMethod = e.m_logMethod;
				e.m_logMethod = nullptr;
				ska::LogEntry::operator=(std::move(e));
				return *this;
			}
            
            ~LogEntry() {
				if (m_logMethod != nullptr) {
					//MUST NOT throw !
					m_logMethod->log(std::move(*this));
				}
            }
        };

        template <class LogMethod>
        std::size_t LogEntry<LogMethod>::InstanceCounter = 0u;
    }
}

