#include "LogSync.h"
#include "LoggerImpl.h"

void ska::LogSync::log(LogEntry& entry, loggerdetail::Logger& logger) {
	logger.consumeNow(entry);
}
