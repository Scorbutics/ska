#pragma once

#include "LogEntry.h"

namespace ska {
    class LogSync {
    public:
        void log(LogEntry entry) {
            entry.consumeTokens();
        }
    };
}
