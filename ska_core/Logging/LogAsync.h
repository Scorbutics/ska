#pragma once

#include <optional>

#include "ActiveObject.h"
#include "LogEntry.h"

namespace ska {
    class LogAsync {
    private:
        std::optional<LogEntry> m_entry;
        ActiveObject m_commander;
    public:
        void log(LogEntry entry) {
            m_entry = std::move(entry);
            m_commander.send([this]() {
                m_entry.value().consumeTokens();
            });
        }
    };
}

