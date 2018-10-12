#pragma once
#include <functional>

namespace ska {
    class LogEntry;
    using LogFilter = std::function<bool(const LogEntry&)>;
    namespace loggerdetail {
        LogFilter GetIdentityLogFilter();
    }
}
