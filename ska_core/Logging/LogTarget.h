#pragma once

#include <functional>
#include <ostream>

namespace ska {
    namespace loggerdetail {
        class LogEntry;
        class Token;

        using LogFilter = std::function<bool(const LogEntry&)>;

        class LogTarget {
        public:
            LogTarget(std::ostream& output, LogFilter filter = GetIdentityLogFilter()) : 
                m_output(output),
                m_filter(std::move(filter)) {
            }

            bool applyTokenOnOutput(const LogEntry& entry, const Token& token);
        
        private:
            bool isATarget(const LogEntry& entry);
            static LogFilter GetIdentityLogFilter();

            std::ostream& m_output;
            LogFilter m_filter;
        };

        
    }
}
