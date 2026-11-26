#pragma once

#include <ostream>

#include "LogFilter.h"
#include "TokenConsumeType.h"

namespace ska {
    class LogEntry;
    namespace loggerdetail {
        struct Token;

        class LogTarget {
        public:
            LogTarget(std::ostream& output, LogFilter filter = GetIdentityLogFilter(), bool supportsComplexLogging = false, bool supportsColoring = false) : 
                m_output(output),
                m_filter(std::move(filter)),
				m_supportsColoring(supportsColoring),
                m_supportsComplexLogging(supportsComplexLogging) {
            }

            TokenConsumeType applyTokenOnOutput(const LogEntry& entry, const Token& token);
            void enableComplexLogging() { m_supportsComplexLogging = true; }
			bool isATarget(const LogEntry& entry);
			void endLine();

        private:
            std::ostream& m_output;
            LogFilter m_filter;
			bool m_supportsColoring;
            bool m_supportsComplexLogging;
        };

        
    }
}
