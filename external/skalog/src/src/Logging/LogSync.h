#pragma once

#include "LogEntry.h"

namespace ska {
	namespace loggerdetail {
		class Logger;
	}

    class LogSync {
    public:
		void log(LogEntry& entry, loggerdetail::Logger& logger);
		void terminate() {}
    };
}
