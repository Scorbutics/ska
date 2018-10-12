#pragma once

#include <ostream>

#include "LogFilter.h"

namespace ska {
    class LogEntry;
    namespace loggerdetail {
        class Token;

        class LogTarget {
        public:
            LogTarget(std::ostream& output, LogFilter filter = GetIdentityLogFilter()) : 
                m_output(output),
                m_filter(std::move(filter)) {
            }

            bool applyTokenOnOutput(const LogEntry& entry, const Token& token);
        
        private:
            bool isATarget(const LogEntry& entry);    

            std::ostream& m_output;
            LogFilter m_filter;
        };

        
    }
}
